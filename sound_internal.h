﻿  // @file sound_internal.h
  // @brief Declaration of sound related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef SOUND_INTERNAL_H_
#define SOUND_INTERNAL_H_
#include <dsound.h>
#include <process.h>
#include <vector>
#include "./common.h"
#include "./common_internal.h"
#include "./sound.h"
  //
  // These are internal macros related to sound
  //
#define SYS_STREAMING_READ_BYTES  (100000)

  //
  // These are internal enumerations and constants related to sound
  //

namespace sys {
  //
  // These are internal structures related to sound
  //
struct WaveData {
  IDirectSoundBuffer* buffer;
  WaveData();
  void Release();
  bool IsNull();
};
struct StreamingData {
  ResourceDesc resource_desc;
  HANDLE hthread;
  bool stop_request;
  bool in_pause;
  bool in_use;
  bool in_loop;
  WaveData wave_data;
  StreamingData();
  void Reset();
};
struct SoundData {
  IDirectSound8* direct_sound8;
  HANDLE hmutex;
  std::vector<WaveData> wave_buffer;
  StreamingData streaming_data;
  IdServer wave_id_server;
  SoundData();
};
extern SoundData sound_data;

  //
  // These are internal functions related to sound
  //
bool InitSound();
void FinalizeSound();
bool UpdateSound();
}  // namespace sys
#endif  // SOUND_INTERNAL_H_
