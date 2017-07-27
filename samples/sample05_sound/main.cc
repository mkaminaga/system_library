// @file main.h
// @brief Wave test.
// @author Mamoru Kaminaga
// @date 2017-06-18 20:59:17
// Copyright 2017 Mamoru Kaminaga
#include "../../system.h"
#include "resource.h"
struct WaveTest {
  int id_a, id_b, id_c;
  int frame;
  int last_frame;
  sys::WaveDesc a, b, c;
  WaveTest() : frame(0), last_frame(0) {
    a.resource_desc.use_mem = false;
    a.resource_desc.file_name = L"sound_a.wav";
    b.resource_desc.use_mem = false;
    b.resource_desc.file_name = L"sound_b.wav";
    c.resource_desc.use_mem = false;
    c.resource_desc.file_name = L"sound_c.wav";
  }
  bool Init() {
    if (!sys::CreateWave(a, &id_a)) return false;
    if (!sys::CreateWave(b, &id_b)) return false;
    if (!sys::CreateWave(c, &id_c)) return false;
    return true;
  }
  bool Finalize() {
    sys::ReleaseWave(id_a);
    sys::ReleaseWave(id_b);
    sys::ReleaseWave(id_c);
    return true;
  }
  bool Play() {
    const int delta = frame - last_frame;
    switch (delta) {
      case 1:
        sys::PlayWave(id_a);
        break;
      case 61:
        sys::PlayWave(id_b);
        break;
      case 121:
        sys::PlayWave(id_c);
        break;
      case 181:
        last_frame = frame;
        break;
      default:
        break;
    }
    ++frame;
    return true;
  }
};
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hinstance_previous,
                    LPTSTR cmdline, int cmdshow) {
  sys::SetWindowTitle(L"System test:sound");
  sys::SetWindowIcon(IDI_ICON1);
  sys::SetWindowSize(640, 480);
  sys::SetResolution(648, 480);
  sys::StartWithFullscreen(false);
  if (!sys::InitSystem(hinstance)) return -1;
  WaveTest test;
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
