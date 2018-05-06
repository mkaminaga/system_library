  // @file sound
  // @brief Declaration of sound related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef SOUND_H_
#define SOUND_H_
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <string>
#include "./common.h"
  //
  // These are public macros related to sound
  //
#define SYS_ERROR_WAVE_ID_EXCEEDS_LIMIT   L"Error! Wave id exceeds limit"
#define SYS_ERROR_DUPLICATE_WAVE_ID       L"Error! Duplicate wave id:%d"
#define SYS_ERROR_NULL_WAVE_ID            L"Error! Null wave id:%d"
#define SYS_ERROR_INVALID_WAVE_ID         L"Error! Invalid wave id:%d"
#define SYS_ERROR_TOO_MANY_WAVE_ID        L"Error! Too many waves, max:%d"

  //
  // These are public enumerations and constants related to sound
  //

namespace sys {
  //
  // These are public structures related to sound
  //
struct WaveDesc {
  ResourceDesc resource_desc;
  WaveDesc() : resource_desc() { }
};
struct StreamingDesc {
  ResourceDesc resource_desc;
  bool use_loop;
  StreamingDesc() :
    resource_desc(),
    use_loop(false) { }
};

  //
  // These are public functions related to sound
  //
bool CreateWave(const WaveDesc& desc, int* wave_id);
bool ReleaseWave(int wave_id);
bool PlayWave(int wave_id);
bool StopWave(int wave_id);
bool PlayStreaming(const StreamingDesc& desc);
bool PauseStreaming();
bool ContinueStreaming();
bool StopStreaming();
}  // namespace sys
#endif  // SOUND_H_
