  // @file common
  // @brief Definitions of common related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include "./common.h"
#include "./common_internal.h"
#include "./system_internal.h"
namespace sys {
  //
  // These are structures related to common
  //
ResourceDesc::ResourceDesc() : file_name(), mem_ptr(nullptr), mem_size(0),
    use_mem(false) { }

  //
  // These are internal functions related to common
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
