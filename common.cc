  // @file common
  // @brief Definitions of common related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include "./common.h"
#include "./common_internal.h"
#include "./system_internal.h"
  //
  // These are public macros related to common
  //

  //
  // These are public enumerations and constants related to common
  //

namespace sys {
  //
  // These are public structures related to common
  //

  //
  // These are private functions related to common
  //

  //
  // These are public functions related to common
  //
bool ErrorDialogBox(const wchar_t* format, ...) {
  wchar_t buffer[256] = {0};
  va_list args;
  va_start(args, format);
  vswprintf_s(buffer, 256, format, args);
  MessageBox(system_data.hwnd, buffer, L"System Error", MB_ICONEXCLAMATION);
  return true;
}
}
