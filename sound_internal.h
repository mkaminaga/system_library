  // @file sound_internal.h
  // @brief Declaration of sound related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef SOUND_INTERNAL_H_
#define SOUND_INTERNAL_H_
#include "./common.h"
#include "./common_internal.h"
#include "./sound.h"
#include <dsound.h>
#include <process.h>
  //
  // These are internal macros related to sound
  //
#define SYS_WAVE_BUFFER_ELEM_NUM  (256)
#define SYS_STREAMING_READ_BYTES  (32768)  // 2 ^ 15

  //
  // These are internal enumerations and constants related to sound
  //

namespace sys {
  //
  // These are internal structures related to sound
  //
struct WaveData {
  IDirectSoundBuffer* buffer;
  WaveData() : buffer(nullptr) { }
  void Release() {
    SYS_SAFE_RELEASE(buffer);
  }
  bool IsNull() {
    return (buffer == nullptr);
  }
};
struct StreamingData {
  ResourceDesc resource_desc;
  HANDLE hthread;
  bool stop_request;
  bool in_pause;
  bool in_use;
  bool in_loop;
  WaveData wave_data;
  StreamingData() : resource_desc(), hthread(nullptr), wave_data(),
      stop_request(false), in_pause(false), in_use(false), in_loop(false) { }
  void Reset() {
    hthread = nullptr;
    stop_request = false;
    in_pause = false;
    in_use = false;
    in_loop = false;
    wave_data.Release();
  }
};
struct SoundData {
  IDirectSound8* direct_sound8;
  HANDLE hmutex;
  WaveData wave_buffer[SYS_WAVE_BUFFER_ELEM_NUM];
  StreamingData streaming_data;
  SoundData() : direct_sound8(nullptr), hmutex(nullptr), wave_buffer(),
      streaming_data() { }
};

  //
  // These are internal functions related to sound
  //
bool InitSound();
void FinalizeSound();
bool UpdateSound();
}
extern sys::SoundData sound_data;
#endif  // SOUND_INTERNAL_H_
