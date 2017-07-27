  // @file sound
  // @brief Definitions of sound related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include "./sound.h"
#include "./sound_internal.h"
#include "./system_internal.h"
#include <memory>
namespace sys {
  //
  // These are structures related to sound
  //
SoundData sound_data;
WaveData::WaveData() : buffer(nullptr) { }
void WaveData::Release() {
  SYS_SAFE_RELEASE(buffer);
}
bool WaveData::IsNull() {
  return (buffer == nullptr);
}
StreamingData::StreamingData() : resource_desc(), hthread(nullptr),
    wave_data(), stop_request(false), in_pause(false), in_use(false),
    in_loop(false) { }
void StreamingData::Reset() {
  hthread = nullptr;
  stop_request = false;
  in_pause = false;
  in_use = false;
  in_loop = false;
  wave_data.Release();
}
SoundData::SoundData() : direct_sound8(nullptr), hmutex(nullptr),
    wave_buffer(), streaming_data() { }

  //
  // These are private functions related to sound
  //
HMMIO GetFileHandle(const wchar_t* file_name) {
  return mmioOpen((LPWSTR) file_name, nullptr, MMIO_READ);
}
HMMIO GetFileHandle(const HPSTR mem_ptr, int mem_size) {
  MMIOINFO mmio_info;
  memset(&mmio_info, 0, sizeof(mmio_info));
  mmio_info.pchBuffer = mem_ptr;
  mmio_info.fccIOProc = FOURCC_MEM;
  mmio_info.cchBuffer = mem_size;
  mmio_info.adwInfo[0] = 1024;
  return mmioOpen(nullptr, &mmio_info, MMIO_READ);
}
bool DescendToRiffChunk(HMMIO hmmio, MMCKINFO* riff_chunk_out) {
  if (!riff_chunk_out) return false;
  memset(riff_chunk_out, 0, sizeof(MMCKINFO));
  riff_chunk_out->fccType = mmioFOURCC('W', 'A', 'V', 'E');
  if (mmioDescend(hmmio, riff_chunk_out, nullptr, MMIO_FINDRIFF) !=
      MMSYSERR_NOERROR) {
    return false;
  }
  return true;
}
bool DescendToFormatChunk(HMMIO hmmio, const MMCKINFO& riff_chunk,
                          MMCKINFO* format_chunk_out) {
  memset(format_chunk_out, 0, sizeof(MMCKINFO));
  format_chunk_out->ckid = mmioFOURCC('f', 'm', 't', ' ');
  if (mmioDescend(hmmio, format_chunk_out, &riff_chunk, MMIO_FINDCHUNK) !=
      MMSYSERR_NOERROR) {
    return false;
  }
  return true;
}
bool ReadWavFormatEx(HMMIO hmmio, const MMCKINFO& format_chunk,
                     WAVEFORMATEX* wave_fmt_ex_out) {
  memset(wave_fmt_ex_out, 0, sizeof(WAVEFORMATEX));
  if (mmioRead(hmmio, (HPSTR) wave_fmt_ex_out, format_chunk.cksize) !=
      static_cast<LONG>(format_chunk.cksize)) {
    return false;
  }
  return true;
}
bool DescendToDataChunk(HMMIO hmmio, const MMCKINFO& riff_chunk,
                        MMCKINFO* data_chunk_out) {
  memset(data_chunk_out, 0, sizeof(MMCKINFO));
  data_chunk_out->ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (mmioDescend(hmmio, data_chunk_out, &riff_chunk, MMIO_FINDCHUNK) !=
      MMSYSERR_NOERROR) {
    return false;
  }
  return true;
}
bool CreateWaveDataBuffer(const char* wave_mem, int wave_data_size,
                          WaveData* wave, WAVEFORMATEX* wave_fmt_ex) {
  assert(wave);
  //
  DSBUFFERDESC ds_buf_desc;
  memset(&ds_buf_desc, 0, sizeof(ds_buf_desc));
  ds_buf_desc.dwSize = sizeof(ds_buf_desc);
  ds_buf_desc.dwFlags = DSBCAPS_GLOBALFOCUS;
  ds_buf_desc.dwBufferBytes = wave_data_size;
  ds_buf_desc.lpwfxFormat = static_cast<LPWAVEFORMATEX>(wave_fmt_ex);
  ds_buf_desc.guid3DAlgorithm = GUID_NULL;
  IDirectSoundBuffer* temp_buffer;
  if (FAILED(
        sound_data.direct_sound8->CreateSoundBuffer(
          &ds_buf_desc,
          &temp_buffer,
          nullptr))) {
    return false;
  }
  temp_buffer->QueryInterface(
      IID_IDirectSoundBuffer8,
      reinterpret_cast<void**>(&wave->buffer));
  SYS_SAFE_RELEASE(temp_buffer);
  //
  if (wave->buffer == nullptr) return false;
  LPVOID write_ptr = nullptr;
  DWORD length = 0;
  if (FAILED(
        wave->buffer->Lock(
          0,
          0,
          &write_ptr,
          &length,
          nullptr,
          nullptr,
          DSBLOCK_ENTIREBUFFER))) {
    return false;
  }
  memcpy(write_ptr, wave_mem, length);
  wave->buffer->Unlock(write_ptr, length, nullptr, 0);
  return true;
}
bool CreateWaveData(const WaveDesc& desc, WaveData* wave) {
  assert(wave);
  const ResourceDesc resource_desc = desc.resource_desc;
  HMMIO hmmio = nullptr;
  if (resource_desc.use_mem) {
    // From memory
    hmmio = GetFileHandle(
        reinterpret_cast<const HPSTR>(resource_desc.mem_ptr),
        resource_desc.mem_size);
  } else {
    // From file
    hmmio = GetFileHandle(resource_desc.file_name.c_str());
  }
  if (hmmio == nullptr) return false;
  MMCKINFO riff_chunk = {0};
  if (!DescendToRiffChunk(hmmio, &riff_chunk)) {
    mmioClose(hmmio, 0);
    return false;
  }
  MMCKINFO format_chunk = {0};
  if (!DescendToFormatChunk(hmmio, riff_chunk, &format_chunk)) {
    mmioClose(hmmio, 0);
    return false;
  }
  WAVEFORMATEX wave_fmt_ex = {0};
  if (!ReadWavFormatEx(hmmio, format_chunk, &wave_fmt_ex)) {
    mmioClose(hmmio, 0);
    return false;
  }
  mmioAscend(hmmio, &format_chunk, 0);
  MMCKINFO data_chunk = {0};
  if (!DescendToDataChunk(hmmio, riff_chunk, &data_chunk)) {
    mmioClose(hmmio, 0);
    return false;
  }
  std::unique_ptr<char[]> wave_mem(new char[data_chunk.cksize]);
  if (mmioRead(hmmio, (HPSTR) wave_mem.get(), data_chunk.cksize) !=
      static_cast<LONG>(data_chunk.cksize)) {
    mmioClose(hmmio, 0);
    return false;
  }
  mmioClose(hmmio, 0);
  // Sound buffer is set.
  WaitForSingleObject(sound_data.hmutex, 0);
  if (!CreateWaveDataBuffer(
        wave_mem.get(),
        data_chunk.cksize,
        wave,
        &wave_fmt_ex)) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool StopWaveData(WaveData* wave) {
  assert(wave);
  WaitForSingleObject(sound_data.hmutex, 0);
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  wave->buffer->Stop();
  wave->buffer->SetCurrentPosition(0);
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool ReleaseWaveData(WaveData* wave) {
  assert(wave);
  StopWaveData(wave);
  WaitForSingleObject(sound_data.hmutex, 0);
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  wave->Release();
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool PlayWaveData(WaveData* wave) {
  assert(wave);
  WaitForSingleObject(sound_data.hmutex, 0);
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  wave->buffer->Play(0, 0, 0);
  ReleaseMutex(sound_data.hmutex);
  return true;
}
unsigned __stdcall StreamingProc(LPVOID lpargs) {
  assert(lpargs);
  ResourceDesc* resource_desc = static_cast<ResourceDesc*>(lpargs);
  HMMIO hmmio = nullptr;
  do {
    if (resource_desc->use_mem) {
      // Form memory
      hmmio = GetFileHandle(
          reinterpret_cast<const HPSTR>(resource_desc->mem_ptr),
          resource_desc->mem_size);
    } else {
      // From file
      hmmio = GetFileHandle(resource_desc->file_name.c_str());
    }
    if (hmmio == nullptr) return false;
    MMCKINFO riff_chunk = {0};
    if (!DescendToRiffChunk(hmmio, &riff_chunk)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    MMCKINFO format_chunk = {0};
    if (!DescendToFormatChunk(hmmio, riff_chunk, &format_chunk)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    WAVEFORMATEX wave_fmt_ex = {0};
    if (!ReadWavFormatEx(hmmio, format_chunk, &wave_fmt_ex)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    mmioAscend(hmmio, &format_chunk, 0);
    MMCKINFO data_chunk = {0};
    if (!DescendToDataChunk(hmmio, riff_chunk, &data_chunk)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    DWORD buf_status = 0;
    std::unique_ptr<char[]> wave_mem(new char[SYS_STREAMING_READ_BYTES]);
    int read_size = 0;
    while (true) {
      if (sound_data.streaming_data.stop_request) {
        mmioClose(hmmio, 0);
        return S_OK;  // Thread terminated.
        break;
      }
      if (!sound_data.streaming_data.in_pause &&
          !(buf_status & DSBSTATUS_PLAYING)) {
        read_size = mmioRead(hmmio, (HPSTR) wave_mem.get(),
                             SYS_STREAMING_READ_BYTES);
        if (read_size == 0) {
          break;
        }
        if (!CreateWaveDataBuffer(
              wave_mem.get(),
              read_size,
              &sound_data.streaming_data.wave_data,
              &wave_fmt_ex)) {
          break;
        }
        PlayWaveData(&sound_data.streaming_data.wave_data);
      }
      sound_data.streaming_data.wave_data.buffer->GetStatus(&buf_status);
    }
    mmioClose(hmmio, 0);
  } while (sound_data.streaming_data.in_loop);
  return S_OK;  // Thread terminated.
}
bool InitSound() {
  if (FAILED(
        DirectSoundCreate8(
          nullptr,
          &sound_data.direct_sound8,
          nullptr))) {
    return false;
  }
  // Cooperative mode is designated.
  if (FAILED(
        sound_data.direct_sound8->SetCooperativeLevel(
          system_data.hwnd,
          DSSCL_PRIORITY))) {
    return false;
  }
  sound_data.streaming_data.Reset();
  sound_data.hmutex = CreateMutex(nullptr, TRUE, nullptr);
  return true;
}
void FinalizeSound() {
  StopStreaming();
  CloseHandle(sound_data.hmutex);
  SYS_SAFE_RELEASE(sound_data.direct_sound8);
}
bool UpdateSound() {
  return true;  // No Impl.
}

  //
  // These are public functions related to sound
  //
bool CreateWave(const WaveDesc& desc, int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Duplicate check (Wave)
  if (!sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_DUPLICATE_WAVE_ID, wave_id);
    return false;
  }
  CreateWaveData(desc, &sound_data.wave_buffer[wave_id]);
  return true;
}
bool StopWave(int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Null check (Wave)
  if (sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_WAVE_ID, wave_id);
    return false;
  }
  return StopWaveData(&sound_data.wave_buffer[wave_id]);
}
bool ReleaseWave(int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Null check (Wave)
  if (sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_WAVE_ID, wave_id);
    return false;
  }
  return ReleaseWaveData(&sound_data.wave_buffer[wave_id]);
}
bool PlayWave(int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Null check (Wave)
  if (sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_WAVE_ID, wave_id);
    return false;
  }
  return PlayWaveData(&sound_data.wave_buffer[wave_id]);
}
bool PlayStreaming(const StreamingDesc& desc) {
  if (sound_data.streaming_data.in_use) return false;
  sound_data.streaming_data.Reset();
  //
  sound_data.streaming_data.resource_desc = desc.resource_desc;
  sound_data.streaming_data.in_loop = desc.use_loop;
  unsigned thread_id = 0;
  sound_data.streaming_data.hthread =
    (HANDLE) _beginthreadex(
      nullptr,
      0,
      StreamingProc,
      &(sound_data.streaming_data.resource_desc),
      0,
      &thread_id);  // NOLINT
  if (sound_data.streaming_data.hthread == nullptr) return false;
  sound_data.streaming_data.in_use = true;
  return true;
}
bool PauseStreaming() {
  if (!sound_data.streaming_data.in_use) return false;
  WaitForSingleObject(sound_data.hmutex, 0);
  if (sound_data.streaming_data.wave_data.IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  StopWaveData(&sound_data.streaming_data.wave_data);
  sound_data.streaming_data.in_pause = true;
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool ContinueStreaming() {
  if (!sound_data.streaming_data.in_use) return false;
  WaitForSingleObject(sound_data.hmutex, 0);
  if (sound_data.streaming_data.wave_data.IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  PlayWaveData(&sound_data.streaming_data.wave_data);
  sound_data.streaming_data.in_pause = false;
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool StopStreaming() {
  if (!sound_data.streaming_data.in_use) return false;
  WaitForSingleObject(sound_data.hmutex, 0);
  sound_data.streaming_data.stop_request = true;
  ReleaseMutex(sound_data.hmutex);
  //
  WaitForSingleObject(sound_data.streaming_data.hthread, INFINITE);
  CloseHandle(sound_data.streaming_data.hthread);
  ReleaseWaveData(&sound_data.streaming_data.wave_data);
  sound_data.streaming_data.Reset();
  return true;
}
}
