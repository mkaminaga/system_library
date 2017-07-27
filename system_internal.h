  // @file system_internal.cc
  // @brief Declaration of system related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef SYSTEM_INTERNAL_H_
#define SYSTEM_INTERNAL_H_
#include "./common.h"
#include "./common_internal.h"
#include "./system.h"
#include <ShellScalingAPI.h>
#include <mmsystem.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <Vecmath.h>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
  //
  // These are internal macros related to system
  //
#define SYS_WINDOW_STYLE    (WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX)
#define SYS_WM_APP_STOP     (WM_APP + 1)
#define SYS_FPS_SAMPLE_NUM  (4)

  //
  // These are internal enumerations and constants related to system
  //

namespace sys {
  //
  // These are internal structures related to system
  //
struct SystemData {
  HINSTANCE hinstance;
  HWND hwnd;
  Vector2<int> window_size;
  std::wstring window_title;
  int icon_id;
  int fps_last_ms;
  int fps_timer_id;
  int last_ms;
  int ms;
  int one_loop_ms;
  int timer_int_ms;
  double fps;
  bool window_forcus;
  bool is_stopped;
  std::list<double> fps_sample;
  SystemData();
};
extern SystemData system_data;

  //
  // These are internal functions related to system
  //
}
#endif  // SYSTEM_INTERNAL_H_
