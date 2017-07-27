  // @file input_internal.h
  // @brief Declaration of input related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef INPUT_INTERNAL_H_
#define INPUT_INTERNAL_H_
#include "./common.h"
#include "./common_internal.h"
#include "./input.h"
#include <dinput.h>
#include <map>
  //
  // These are internal macros related to input
  //
#define SYS_JOYPAD_RANGE_MAX          (1000)
#define SYS_JOYPAD_THRESHOLD_DEFAULT  (50)

  //
  // These are internal enumerations and constants related to input
  //

namespace sys {
  //
  // These are internal structures related to input
  //
struct InputStatus {
  virtual ~InputStatus() { }
  virtual bool IsON(size_t key) = 0;
  virtual void Reset() = 0;
};
struct KeyboardStatus : public InputStatus {
  char status[SYS_ELEMNUM_KEYID];
  KeyboardStatus();
  KeyboardStatus(const KeyboardStatus& keyboard_status);
  bool IsON(size_t key);
  void Reset();
};
struct JoypadStatus : public InputStatus {
  char status[32];
  int analog_stick[2];
  int threshold_x;
  int threshold_y;
  JoypadStatus();
  JoypadStatus(const JoypadStatus& joypad_status);
  bool IsON(size_t key);
  void Reset();
};
struct VirtualStatus : public InputStatus {
  char status[SYS_ELEMNUM_BUTTON_KEY];
  VirtualStatus();
  VirtualStatus(const VirtualStatus& virtual_status);
  bool IsON(size_t key);
  void Reset();
};
struct InputData {
  IDirectInput8* keyboard8;
  IDirectInput8* joypad8;
  IDirectInputDevice8* keyboard_device;
  IDirectInputDevice8* joypad_device;
  //
  bool keyboard_available;
  bool joypad_available;
  KeyboardStatus keyboard_status;
  JoypadStatus joypad_status;
  VirtualStatus virtual_status;
  VirtualStatus last_virtual_status;
  VirtualStatus virtual_pressed;
  std::map<int, int> key_map;  // <key, button key>
  std::map<int, int> joypad_key_map;  // <joypad key, button key>
  InputData();
};

  //
  // These are internal functions related to input
  //
bool InitInput();
void FinalizeInput();
bool UpdateInput();
}
extern sys::InputData input_data;
#endif  // INPUT_INTERNAL_H_
