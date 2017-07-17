// @file main.h
// @brief Streaming test.
// @author Mamoru Kaminaga
// @date 2017-06-19 12:18:44
// Copyright 2017 Mamoru Kaminaga
#include "../../system.h"
#include "resource.h"
struct StreamingTest {
  sys::StreamingDesc a;
  StreamingTest() {
    a.resource_desc.use_mem = false;
    a.resource_desc.file_name = L"bgm_maoudamashii_8bit25.wav";
    a.use_loop = true;
  }
  bool Init() {
    if (!sys::PlayStreaming(a)) return false;
    return true;
  }
  bool Finalize() {
    sys::StopStreaming();
    return true;
  }
  bool Play() {
    // No Impl.
    return true;
  }
};
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hinstance_previous,
                    LPTSTR cmdline, int cmdshow) {
  sys::SetWindowTitle(L"System test:streaming");
  sys::SetWindowIcon(IDI_ICON1);
  sys::SetWindowSize(640, 480);
  sys::SetResolution(648, 480);
  sys::StartWithFullscreen(false);
  if (!sys::InitSystem(hinstance)) return -1;
  StreamingTest test;
  if (!test.Init()) return -1;
  while (sys::UpdateSystem()) {
    sys::FillScreen(Color4b(0, 64, 64, 255));
    test.Play();
  }
  test.Finalize();
  sys::FinalizeSystem();
  //
  UNREFERENCED_PARAMETER(hinstance_previous);
  UNREFERENCED_PARAMETER(cmdline);
  UNREFERENCED_PARAMETER(cmdshow);
  return 0;
}
