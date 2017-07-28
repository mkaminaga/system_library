  // @file system
  // @brief Declaration of system related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <wchar.h>
#include <windows.h>
#include "./common.h"
#include "./graphic.h"
#include "./input.h"
#include "./sound.h"
#include "./system.h"
  //
  // These are public macros related to system
  //

  //
  // These are public enumerations and constants related to system
  //

namespace sys {
  //
  // These are public structures related to system
  //

  //
  // These are public functions related to system
  //
bool InitSystem(HINSTANCE hinstance);
void FinalizeSystem();
bool UpdateSystem();
void StopSystem();
void SetWindowTitle(const wchar_t* window_title);
void SetWindowIcon(int icon_id);
void SetWindowSize(int x, int y);
int GetMilliSecond();
double GetFPS();
bool GetForcuse();
}  // namespace sys  // namespace sys
#endif  // SYSTEM_H_
