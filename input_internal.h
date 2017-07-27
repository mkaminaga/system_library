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
  KeyboardStatus() : status() { }
  KeyboardStatus(const KeyboardStatus& keyboard_status) {
    memcpy(status, keyboard_status.status, sizeof(status));
  }
  bool IsON(size_t key) {
    if ((key < 0) || (key >= SYS_ELEMNUM_KEYID)) {
      ErrorDialogBox(SYS_ERROR_INVALID_KEY, key);
      return false;
    }
    return (status[key] != 0);
  }
  void Reset() {
    memset(status, 0, sizeof(status));
  }
};
struct JoypadStatus : public InputStatus {
  char status[32];
  int analog_stick[2];
  int threshold_x;
  int threshold_y;
  JoypadStatus() :
      status(),
      analog_stick(),
      threshold_x(SYS_JOYPAD_THRESHOLD_DEFAULT),
      threshold_y(SYS_JOYPAD_THRESHOLD_DEFAULT) { }
  JoypadStatus(const JoypadStatus& joypad_status) {
    memcpy(status, joypad_status.status, sizeof(status));
    memcpy(analog_stick, joypad_status.analog_stick, sizeof(analog_stick));
  }
  bool IsON(size_t key) {
    switch (key) {
      case SYS_JOYPAD_KEY_DOWN:
        return (analog_stick[1] > threshold_y);
      case SYS_JOYPAD_KEY_LEFT:
        return (analog_stick[0] < -threshold_x);
      case SYS_JOYPAD_KEY_RIGHT:
        return (analog_stick[0] > threshold_x);
      case SYS_JOYPAD_KEY_UP:
        return (analog_stick[1] < -threshold_y);
      default:
        if ((key < 0) || (key >= 32)) {
          ErrorDialogBox(SYS_ERROR_INVALID_JOYPAD_KEY, key);
          return false;
        }
        return (status[key] != 0);
    }
  }
  void Reset() {
    memset(status, 0, sizeof(status));
    memset(analog_stick, 0, sizeof(analog_stick));
  }
};
struct VirtualStatus : public InputStatus {
  char status[SYS_ELEMNUM_BUTTON_KEY];
  VirtualStatus() : status() { }
  VirtualStatus(const VirtualStatus& virtual_status) {
    memcpy(status, virtual_status.status, sizeof(status));
  }
  bool IsON(size_t key) {
    if ((key < 0) || (key >= SYS_ELEMNUM_BUTTON_KEY)) {
      ErrorDialogBox(SYS_ERROR_INVALID_VIRTUAL_KEY, key);
      return false;
    }
    return (status[key] != 0);
  }
  void Reset() {
    memset(status, 0, sizeof(status));
  }
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
  InputData() :
      keyboard8(nullptr),
      joypad8(nullptr),
      keyboard_device(nullptr),
      joypad_device(nullptr),
      keyboard_available(false),
      joypad_available(false) {
    // Fixed virtual key assign for keyboard.
    key_map.insert(std::make_pair(SYS_KEY_DOWN, SYS_VIRTUAL_KEY_DOWN));
    key_map.insert(std::make_pair(SYS_KEY_LEFT, SYS_VIRTUAL_KEY_LEFT));
    key_map.insert(std::make_pair(SYS_KEY_RIGHT, SYS_VIRTUAL_KEY_RIGHT));
    key_map.insert(std::make_pair(SYS_KEY_UP, SYS_VIRTUAL_KEY_UP));
    // Fixed virtual key assign for joypad.
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_DOWN, SYS_VIRTUAL_KEY_DOWN));
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_LEFT, SYS_VIRTUAL_KEY_LEFT));
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_RIGHT, SYS_VIRTUAL_KEY_RIGHT));
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_UP, SYS_VIRTUAL_KEY_UP));
  }
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
