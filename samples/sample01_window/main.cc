// @file main.h
// @brief Plain window.
// @author Mamoru Kaminaga
// @date 2017-06-17 16:18:40
// Copyright 2017 Mamoru Kaminaga
#include "../../system.h"
#include "resource.h"
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hinstance_previous,
                    LPTSTR cmdline, int cmdshow) {
  sys::SetWindowTitle(L"System test:plain window");
  sys::SetWindowIcon(IDI_ICON1);
  sys::SetWindowSize(640, 480);
  sys::SetResolution(648, 480);
  sys::StartWithFullscreen(false);
  if (!sys::InitSystem(hinstance)) return -1;
  while (sys::UpdateSystem()) {
    if (sys::GetMilliSecond() > 10000) {
       sys::ErrorDialogBox(L"10 seconds passed, exit main loop");
       sys::StopSystem();
    }
  }
  sys::FinalizeSystem();
  //
  UNREFERENCED_PARAMETER(hinstance_previous);
  UNREFERENCED_PARAMETER(cmdline);
  UNREFERENCED_PARAMETER(cmdshow);
  return 0;
}
