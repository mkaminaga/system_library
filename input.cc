  // @file input
  // @brief Definitions of input related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include <map>
#include "./input.h"
#include "./input_internal.h"
#include "./system_internal.h"
namespace sys {
  //
  // These are internal structures related to input
  //
InputData input_data;
KeyboardStatus::KeyboardStatus() : status() { }
KeyboardStatus::KeyboardStatus(const KeyboardStatus& keyboard_status) {
  memcpy(status, keyboard_status.status, sizeof(status));
}
bool KeyboardStatus::IsON(size_t key) {
  if ((key < 0) || (key >= SYS_ELEMNUM_KEYID)) {
    ErrorDialogBox(SYS_ERROR_INVALID_KEY, key);
    return false;
  }
  return (status[key] != 0);
}
void KeyboardStatus::Reset() {
  memset(status, 0, sizeof(status));
}
JoypadStatus::JoypadStatus() :
    status(),
    analog_stick(),
    threshold_x(SYS_JOYPAD_THRESHOLD_DEFAULT),
    threshold_y(SYS_JOYPAD_THRESHOLD_DEFAULT) { }
JoypadStatus::JoypadStatus(const JoypadStatus& joypad_status) {
  memcpy(status, joypad_status.status, sizeof(status));
  memcpy(analog_stick, joypad_status.analog_stick, sizeof(analog_stick));
}
bool JoypadStatus::IsON(size_t key) {
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
void JoypadStatus::Reset() {
  memset(status, 0, sizeof(status));
  memset(analog_stick, 0, sizeof(analog_stick));
}
VirtualStatus::VirtualStatus() : status() { }
VirtualStatus::VirtualStatus(const VirtualStatus& virtual_status) {
  memcpy(status, virtual_status.status, sizeof(status));
}
bool VirtualStatus::IsON(size_t key) {
  if ((key < 0) || (key >= SYS_ELEMNUM_BUTTON_KEY)) {
    ErrorDialogBox(SYS_ERROR_INVALID_VIRTUAL_KEY, key);
    return false;
  }
  return (status[key] != 0);
}
void VirtualStatus::Reset() {
  memset(status, 0, sizeof(status));
}
InputData::InputData() :
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

  //
  // These are public structures related to input
  //

  //
  // These are private functions related to input
  //
bool AquireKeyboard() {
  if (!input_data.keyboard_device) return false;
  if (FAILED(input_data.keyboard_device->Acquire())) return false;
  return true;
}
bool AquireJoypad() {
  if (!input_data.joypad_device) return false;
  if (FAILED(input_data.joypad_device->Acquire())) return false;
  return true;
}
bool InitKeyboard() {
  if (FAILED(
        CoCreateInstance(
          CLSID_DirectInput8,
          nullptr,
          CLSCTX_INPROC_SERVER,
          IID_IDirectInput8,
          reinterpret_cast<void**>(&input_data.keyboard8)))) {
    return false;
  }
  if (!input_data.keyboard8) return false;
  if (FAILED(
        input_data.keyboard8->Initialize(
          system_data.hinstance,
          DIRECTINPUT_VERSION))) {
    return false;
  }
  // Keyboard device interface acquired.
  if (FAILED(
        input_data.keyboard8->CreateDevice(
          GUID_SysKeyboard,
          &input_data.keyboard_device,
          nullptr))) {
    return false;
  }
  if (!input_data.keyboard_device) return false;
  // Data format set.
  if (FAILED(
        input_data.keyboard_device->SetDataFormat(
          &c_dfDIKeyboard))) {
    return false;
  }
  // Cooperative mode set.
  if (FAILED(
        input_data.keyboard_device->SetCooperativeLevel(
          system_data.hwnd,
          DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
    return false;
  }
  return true;
}
BOOL CALLBACK EnumJoysticProc(const DIDEVICEINSTANCE* pdidInstance,
                              void* context) {
  if (FAILED(
        input_data.joypad8->CreateDevice(
          pdidInstance->guidInstance,
          &input_data.joypad_device,
          nullptr))) {
    return DIENUM_CONTINUE;
  }
  UNREFERENCED_PARAMETER(context);
  return DIENUM_STOP;
}
BOOL CALLBACK EnumAxisProc(const DIDEVICEOBJECTINSTANCE* pdidoi,
                           void* context) {
  DIPROPRANGE range;
  memset(&range, 0, sizeof(range));
  range.diph.dwSize = sizeof(range);
  range.diph.dwHeaderSize = sizeof(range.diph);
  range.diph.dwHow = DIPH_BYID;
  range.diph.dwObj = pdidoi->dwType;
  range.lMin = -SYS_JOYPAD_RANGE_MAX;
  range.lMax = SYS_JOYPAD_RANGE_MAX;
  if (FAILED(
        input_data.joypad_device->SetProperty(
          DIPROP_RANGE,
          &(range.diph)))) {
    return DIENUM_STOP;
  }
  UNREFERENCED_PARAMETER(context);
  return DIENUM_CONTINUE;
}
int InitJoypad() {
  if (FAILED(
        CoCreateInstance(
          CLSID_DirectInput8,
          nullptr,
          CLSCTX_INPROC_SERVER,
          IID_IDirectInput8,
          reinterpret_cast<void**>(&input_data.joypad8)))) {
    return false;
  }
  if (input_data.joypad8 == nullptr) return false;
  if (FAILED(
        input_data.joypad8->Initialize(
          system_data.hinstance,
          DIRECTINPUT_VERSION))) {
    return false;
  }
  // Pad device interface acquired.
  if (FAILED(
        input_data.joypad8->EnumDevices(
          DI8DEVCLASS_GAMECTRL,
          EnumJoysticProc,
          nullptr,
          DIEDFL_ATTACHEDONLY))) {
    return false;  // No joypad found
  }
  if (input_data.joypad_device == nullptr) return false;
  // Data format set.
  if (FAILED(input_data.joypad_device->SetDataFormat(&c_dfDIJoystick))) {
    return false;
  }
  // Set cooperative mode
  if (FAILED(
        input_data.joypad_device->SetCooperativeLevel(
          system_data.hwnd,
          DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
      return false;
  }
  // Describe pad config
  if (FAILED(
        input_data.joypad_device->EnumObjects(
          EnumAxisProc,
          nullptr,
          DIDFT_AXIS))) {
    return false;
  }
  // Axis mode is set.
  DIPROPDWORD diprop;
  diprop.diph.dwSize = sizeof(diprop);
  diprop.diph.dwHeaderSize = sizeof(diprop.diph);
  diprop.diph.dwObj = 0;
  diprop.diph.dwHow = DIPH_DEVICE;
  diprop.dwData = DIPROPAXISMODE_ABS;
  if (FAILED(
        input_data.joypad_device->SetProperty(
          DIPROP_AXISMODE,
          &diprop.diph))) {
    return false;
  }
  return true;
}
void FinalizeKeyboard() {
  if (input_data.keyboard_device != nullptr) {
    input_data.keyboard_device->Unacquire();
  }
  SYS_SAFE_RELEASE(input_data.keyboard_device);
  SYS_SAFE_RELEASE(input_data.keyboard8);
}
void ReleaseJoypad() {
  if (input_data.joypad_device != nullptr) {
    input_data.joypad_device->Unacquire();
  }
  SYS_SAFE_RELEASE(input_data.joypad_device);
  SYS_SAFE_RELEASE(input_data.joypad8);
}
bool UpdateKeyboardStatus() {
  input_data.keyboard_status.Reset();
  if (FAILED(
        input_data.keyboard_device->GetDeviceState(
          SYS_ELEMNUM_KEYID,
          input_data.keyboard_status.status))) {
    return false;
  }
  return true;
}
bool UpdateJoypadStatus() {
  input_data.joypad_status.Reset();
  if (FAILED(input_data.joypad_device->Poll())) return false;
  DIJOYSTATE jstate;
  memset(&jstate, 0, sizeof(jstate));
  if (FAILED(
        input_data.joypad_device->GetDeviceState(
          sizeof(jstate),
          &jstate))) {
    return false;
  }
  for (int i = 0; i < 32; ++i) {
    input_data.joypad_status.status[i] =
      static_cast<char>(jstate.rgbButtons[i]);
  }
  input_data.joypad_status.analog_stick[0] = static_cast<int>(jstate.lX);
  input_data.joypad_status.analog_stick[1] = static_cast<int>(jstate.lY);
  return true;
}
bool UpdateVirtualInputStatus() {
  input_data.virtual_status.Reset();
  // User assigned
  for (auto it : input_data.key_map) {
    if (input_data.keyboard_status.IsON(it.first)) {
      input_data.virtual_status.status[it.second] = 1;
    }
  }
  for (auto it : input_data.joypad_key_map) {
    if (input_data.joypad_status.IsON(it.first)) {
      input_data.virtual_status.status[it.second] = 1;
    }
  }
  return true;
}
bool UpdateVirtualInputPressed() {
  input_data.virtual_pressed.Reset();
  for (int i = 0; i < SYS_ELEMNUM_BUTTON_KEY; ++i) {
    if ((input_data.virtual_status.status[i] == 1) &&
        (input_data.last_virtual_status.status[i] == 0)) {
      input_data.virtual_pressed.status[i] = 1;
    }
  }
  input_data.last_virtual_status = input_data.virtual_status;
  return true;
}
bool InitInput() {
  if (InitKeyboard()) input_data.keyboard_available = true;
  if (InitJoypad()) input_data.joypad_available = true;
  if (!input_data.keyboard_available && !input_data.joypad_available) {
    ErrorDialogBox(SYS_ERROR_NO_INPUT_DEVICE);
    return false;
  }
  return true;
}
void FinalizeInput() {
  FinalizeKeyboard();
  ReleaseJoypad();
}
bool UpdateInput() {
  if (input_data.keyboard_available) {
    if (AquireKeyboard()) UpdateKeyboardStatus();
  }
  if (input_data.joypad_available) {
    if (AquireJoypad()) UpdateJoypadStatus();
  }
  UpdateVirtualInputStatus();
  UpdateVirtualInputPressed();
  return true;
}

  //
  // These are public functions related to input
  //
bool SetVirtualInputKey(SYS_VIRTUAL_KEY virtual_key, SYS_KEY key,
                        SYS_JOYPAD_KEY joypad_key) {
  switch (virtual_key) {
    case SYS_VIRTUAL_KEY_DOWN:
    case SYS_VIRTUAL_KEY_LEFT:
    case SYS_VIRTUAL_KEY_RIGHT:
    case SYS_VIRTUAL_KEY_UP:
      ErrorDialogBox(SYS_ERROR_ARROW_KEYS_NOT_CUSTOM);
      return false;
    default:
      if ((virtual_key < 0) || (virtual_key >= SYS_ELEMNUM_BUTTON_KEY)) {
        ErrorDialogBox(SYS_ERROR_INVALID_VIRTUAL_KEY, virtual_key);
        return false;
      }
      // Key assign
      if ((key < 0) || (key >= SYS_ELEMNUM_KEYID)) {
        ErrorDialogBox(SYS_ERROR_INVALID_KEY, key);
        return false;
      }
      // Joypad assign
      if ((joypad_key < 0) || (joypad_key >= SYS_ELEMNUM_CONTROLLERID)) {
        ErrorDialogBox(SYS_ERROR_INVALID_JOYPAD_KEY, joypad_key);
        return false;
      }
      input_data.key_map.insert(std::make_pair(key, virtual_key));
      input_data.joypad_key_map.insert(
          std::make_pair(joypad_key, virtual_key));
      break;
  }
  return true;
}
void SetJoypadThreshold(int threshold_x, int threshold_y) {
  if ((threshold_x <= 0) || (threshold_x >= SYS_JOYPAD_RANGE_MAX)) {
    threshold_x = SYS_JOYPAD_THRESHOLD_DEFAULT;
    ErrorDialogBox(SYS_ERROR_INVALID_THRESHOLD, threshold_x);
  }
  input_data.joypad_status.threshold_x = threshold_x;
  if ((threshold_y <= 0) || (threshold_y >= SYS_JOYPAD_RANGE_MAX)) {
    threshold_y = SYS_JOYPAD_THRESHOLD_DEFAULT;
    ErrorDialogBox(SYS_ERROR_INVALID_THRESHOLD, threshold_y);
  }
  input_data.joypad_status.threshold_y = threshold_y;
}
bool GetKeyboardStatus(SYS_KEY key) {
  return input_data.keyboard_status.IsON(key);
}
bool GetJoypadStatus(SYS_JOYPAD_KEY joypad_key) {
  return input_data.joypad_status.IsON(joypad_key);
}
bool GetVirtualInputStatus(SYS_VIRTUAL_KEY virtual_key) {
  return input_data.virtual_status.IsON(virtual_key);
}
bool GetVirtualInputPressed(SYS_VIRTUAL_KEY virtual_key) {
  return input_data.virtual_pressed.IsON(virtual_key);
}
}
