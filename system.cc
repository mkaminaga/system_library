  // @file system
  // @brief Definitions of system related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include "./common.h"
#include "./common_internal.h"
#include "./graphic.h"
#include "./graphic_internal.h"
#include "./input.h"
#include "./input_internal.h"
#include "./sound.h"
#include "./sound_internal.h"
#include "./system.h"
#include "./system_internal.h"
namespace sys {
  //
  // These are internal structures related to system
  //
SystemData system_data;
SystemData::SystemData() : hinstance(nullptr), hwnd(nullptr),
      window_size(640, 480), window_title(L"System"), icon_id(0),
      fps_last_ms(0), ms(0), timer_int_ms(0), fps(0.0), window_forcus(false),
      is_stopped(false) { }

  //
  // These are public structures related to system
  //

  //
  // These are private functions related to system
  //
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case SYS_WM_APP_STOP:  // WM_CLOSE is caused.
    case WM_CLOSE:
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default:
      break;
  }
  return DefWindowProc(hwnd, msg, wp, lp);
}
bool MyRegisterWindowClass() {
  WNDCLASSEX wcex;
  memset(&wcex, 0, sizeof(wcex));
  wcex.cbSize = sizeof(wcex);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = system_data.hinstance;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = system_data.window_title.c_str();
  if (system_data.icon_id != 0) {
    // Custon icon is used.
    wcex.hIcon = LoadIcon(system_data.hinstance,
                          MAKEINTRESOURCE(system_data.icon_id));
    wcex.hIconSm = LoadIcon(system_data.hinstance,
                            MAKEINTRESOURCE(system_data.icon_id));
  } else {
    // Default icon is used.
    wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
  }
  if (RegisterClassEx(&wcex) == 0) return false;
  return true;
}
bool MyCreateWindow() {
  RECT rc = { 0, 0, system_data.window_size.x, system_data.window_size.y };
  AdjustWindowRect(&rc, SYS_WINDOW_STYLE, FALSE);
  SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_SYSTEM_DPI_AWARE);
  system_data.hwnd = CreateWindow(
      system_data.window_title.c_str(),
      system_data.window_title.c_str(),
      SYS_WINDOW_STYLE,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      rc.right - rc.left,
      rc.bottom - rc.top,
      nullptr,
      nullptr,
      system_data.hinstance,
      nullptr);
  if (system_data.hwnd == nullptr) return false;
  ShowWindow(system_data.hwnd, TRUE);
  return true;
}
bool InitCOM() {
  if (FAILED(CoInitializeEx(nullptr, COINIT_MULTITHREADED))) return false;
  return true;
}
void FinalizeCOM() {
  CoUninitialize();
}
bool InitTimerPeriod() {
  TIMECAPS timeCaps;
  timeGetDevCaps(&timeCaps, sizeof(timeCaps));
  timeBeginPeriod(timeCaps.wPeriodMin);
  system_data.timer_int_ms = timeCaps.wPeriodMin;
  if (system_data.timer_int_ms == 0) return false;
  return true;
}
void FinalizeTimerPeriod() {
  timeEndPeriod(system_data.timer_int_ms);
}
void CALLBACK TimerProc(UINT timer_id, UINT msg, DWORD_PTR user,
                        DWORD_PTR ptr1, DWORD_PTR ptr2) {
  system_data.ms += 1;
  // These macro functions are to prevent warnings for unreferenced parameters.
  UNREFERENCED_PARAMETER(timer_id);
  UNREFERENCED_PARAMETER(msg);
  UNREFERENCED_PARAMETER(user);
  UNREFERENCED_PARAMETER(ptr1);
  UNREFERENCED_PARAMETER(ptr2);
}
bool InitTimer() {
  system_data.fps_timer_id = timeSetEvent(
      system_data.timer_int_ms,
      system_data.timer_int_ms,
      TimerProc,
      NULL,
      TIME_PERIODIC);
  if (system_data.fps_timer_id == 0) return false;
  return true;
}
void FinalizeTimer() {
  if (system_data.fps_timer_id != 0) timeKillEvent(system_data.fps_timer_id);
}
bool ProcessMessage() {
  // This function polls when the function sys::StopSystem is called.
  MSG msg;
  while (true) {
    while (true) {
      if ((system_data.ms - system_data.last_ms) >= 16) {
        system_data.last_ms = system_data.ms;
        break;
      }
      // This sleep is here to reduce the load of the CPU.
      Sleep(1);
    }
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) return false;
      // TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    if (!system_data.is_stopped) return true;
  }
}
bool InitFPSCnt() {
  for (int i = SYS_FPS_SAMPLE_NUM; i; --i) {
    system_data.fps_sample.push_back(0);
  }
  return true;
}
void FinalizeFPSCnt() {
  /* No Impl */
}
void UpdateFPSCnt() {
  double fps = 1000.0 /
    static_cast<double>(system_data.ms - system_data.fps_last_ms);
  system_data.fps_last_ms = system_data.ms;
  // Moving average updated.
  system_data.fps_sample.pop_front();
  system_data.fps_sample.push_back(fps);
  double sum = 0.0;
  for (auto& it : system_data.fps_sample) sum += it;
  system_data.fps = sum / SYS_FPS_SAMPLE_NUM;
}

  //
  // These are public functions related to system
  //
bool InitSystem(HINSTANCE hinstance) {
  system_data.hinstance = hinstance;
  if (!InitCOM()) return false;
  //
  if (!MyRegisterWindowClass()) return false;
  if (!MyCreateWindow()) return false;
  if (!InitTimerPeriod()) return false;
  if (!InitTimer()) return false;
  if (!InitFPSCnt()) return false;
  //
  if (!InitGraphic()) return false;
  if (!InitInput()) return false;
  if (!InitSound()) return false;
  return true;
}
void FinalizeSystem() {
  FinalizeSound();
  FinalizeInput();
  FinalizeGraphic();
  //
  FinalizeFPSCnt();
  FinalizeTimer();
  FinalizeTimerPeriod();
  //
  FinalizeCOM();
}
bool UpdateSystem() {
  if (!ProcessMessage()) return false;
  UpdateFPSCnt();
  //
  if (!UpdateGraphic()) StopSystem();
  if (!UpdateInput()) StopSystem();
  if (!UpdateSound()) StopSystem();
  //
  system_data.window_forcus = true;
  if (GetForegroundWindow() != system_data.hwnd) {
    system_data.window_forcus = false;
  }
  return true;
}
void StopSystem() {
  system_data.is_stopped = true;
  SendMessage(system_data.hwnd, SYS_WM_APP_STOP, 0, 0);
}
void SetWindowTitle(const wchar_t* window_title) {
  system_data.window_title = window_title;
}
void SetWindowIcon(int icon_id) {
  system_data.icon_id = icon_id;
}
void SetWindowSize(int x, int y) {
  system_data.window_size.x = x;
  system_data.window_size.y = y;
}
int GetMilliSecond() {
  return system_data.ms;
}
double GetFPS() {
  return system_data.fps;
}
bool GetForcuse() {
  return system_data.window_forcus;
}
}  // namespace sys
