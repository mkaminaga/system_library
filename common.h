  // @file common.h
  // @brief Declaration of sound related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef COMMON_H_
#define COMMON_H_
#include <stdint.h>
#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <string>
  //
  // These are public macros related to sound
  //
#define SYS_SAFE_RELEASE(p) if ((p) != nullptr) {(p)->Release(); (p)=nullptr;}

  //
  // These are public enumerations and constants related to sound
  //

namespace sys {
  //
  // These are public structures related to sound
  //
struct ResourceDesc {
  std::wstring file_name;
  uint8_t* mem_ptr;
  size_t mem_size;
  bool use_mem;
  ResourceDesc();
};

  //
  // These are public functions related to sound
  //
bool ErrorDialogBox(const wchar_t* format, ...);
}  // namespace sys
#endif  // COMMON_H_
