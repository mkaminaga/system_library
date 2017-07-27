// @file main.h
// @brief Font test.
// @author Mamoru Kaminaga
// @date 2017-06-17 16:18:40
// Copyright 2017 Mamoru Kaminaga
#include "../../system.h"
#include "resource.h"
struct FontText {
  int id_x1, id_x2, id_x3;
  sys::FontDesc x1, x2, x3;
  FontText() {
    x1.resource_desc.use_mem = false;
    x1.resource_desc.file_name = L"font.png";
    x1.s = 1.0;
    x2.resource_desc.use_mem = false;
    x2.resource_desc.file_name = L"font.png";
    x2.s = 2.0;
    x3.resource_desc.use_mem = false;
    x3.resource_desc.file_name = L"font.png";
    x3.s = 3.0;
  }
  bool Init() {
    if (!sys::CreateFont(x1, &id_x1)) return false;
    if (!sys::CreateFont(x2, &id_x2)) return false;
    if (!sys::CreateFont(x3, &id_x3)) return false;
    return true;
  }
  bool Finalize() {
    sys::ReleaseFont(id_x1);
    sys::ReleaseFont(id_x2);
    sys::ReleaseFont(id_x3);
    return true;
  }
  bool Paint() {
    Color4b col(0, 0, 0, 0);  // plain color
    Point2d pos_x1(400.0, 100.0);
    Point2d pos_x2(400.0, 200.0);
    Point2d pos_x3(400.0, 300.0);
    sys::DrawText(id_x1, pos_x1, col, SYS_FONTMODE_TOP_LEFT,
                  L"[X1.0 TOP LEFT   ]");
    sys::DrawText(id_x2, pos_x2, col, SYS_FONTMODE_TOP_CENTER,
                  L"[X2.0 TOP CENTER ]");
    sys::DrawText(id_x3, pos_x3, col, SYS_FONTMODE_TOP_RIGHT,
                  L"[X3.0 TOP RIGHT  ]");
    return true;
  }
};
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hinstance_previous,
                    LPTSTR cmdline, int cmdshow) {
  sys::SetWindowTitle(L"System test:font");
  sys::SetWindowIcon(IDI_ICON1);
  sys::SetWindowSize(640, 480);
  sys::SetResolution(648, 480);
  sys::StartWithFullscreen(false);
  if (!sys::InitSystem(hinstance)) return -1;
  FontText test;
  if (!test.Init()) return -1;
  while (sys::UpdateSystem()) {
    sys::FillScreen(Color4b(20, 20, 0, 255));
    test.Paint();
  }
  test.Finalize();
  sys::FinalizeSystem();
  //
  UNREFERENCED_PARAMETER(hinstance_previous);
  UNREFERENCED_PARAMETER(cmdline);
  UNREFERENCED_PARAMETER(cmdshow);
  return 0;
}
