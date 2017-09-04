// @file main.h
// @brief Picture test.
// @author Mamoru Kaminaga
// @date 2017-06-17 16:18:40
// Copyright 2017 Mamoru Kaminaga
#include "../../system.h"
#include "resource.h"
struct PictureTest {
  int id_t;
  int id_a_0, id_a_1, id_a_2, id_a_3, id_a_4;
  int id_b_0, id_b_1, id_b_2, id_b_3, id_b_4;
  int id_c_0, id_c_1, id_c_2, id_c_3, id_c_4;
  int frame;
  int last_frame;
  sys::TextureDesc t;
  sys::ImageDesc a_0, a_1, a_2, a_3, a_4;
  sys::ImageDesc b_0, b_1, b_2, b_3, b_4;
  sys::ImageDesc c_0, c_1, c_2, c_3, c_4;
  PictureTest() : frame(0), last_frame(0) {
    t.resource_desc.use_mem = false;
    t.resource_desc.file_name = L"picture.png";
    a_0.x =   0; a_0.y =   0; a_0.w =  64; a_0.h =  32;
    a_1.x =  64; a_1.y =   0; a_1.w =  64; a_1.h =  32;
    a_2.x = 128; a_2.y =   0; a_2.w =  64; a_2.h =  32;
    a_3.x =   0; a_3.y =  32; a_3.w =  64; a_3.h =  32;
    a_4.x =  64; a_4.y =  32; a_4.w =  64; a_4.h =  32;
    b_0.x =   0; b_0.y =  64; b_0.w =  64; b_0.h =  32;
    b_1.x =  64; b_1.y =  64; b_1.w =  64; b_1.h =  32;
    b_2.x = 128; b_2.y =  64; b_2.w =  64; b_2.h =  32;
    b_3.x =   0; b_3.y =  96; b_3.w =  64; b_3.h =  32;
    b_4.x =  64; b_4.y =  96; b_4.w =  64; b_4.h =  32;
    c_0.x =   0; c_0.y = 128; c_0.w =  64; c_0.h =  32;
    c_1.x =  64; c_1.y = 128; c_1.w =  64; c_1.h =  32;
    c_2.x = 128; c_2.y = 128; c_2.w =  64; c_2.h =  32;
    c_3.x =   0; c_3.y = 160; c_3.w =  64; c_3.h =  32;
    c_4.x =  64; c_4.y = 160; c_4.w =  64; c_4.h =  32;
  }
  bool Init() {
    if (!sys::CreateTexture(t, &id_t)) return false;
    if (!sys::CreateImage(a_0, &id_a_0)) return false;
    if (!sys::CreateImage(a_1, &id_a_1)) return false;
    if (!sys::CreateImage(a_2, &id_a_2)) return false;
    if (!sys::CreateImage(a_3, &id_a_3)) return false;
    if (!sys::CreateImage(a_4, &id_a_4)) return false;
    if (!sys::CreateImage(b_0, &id_b_0)) return false;
    if (!sys::CreateImage(b_1, &id_b_1)) return false;
    if (!sys::CreateImage(b_2, &id_b_2)) return false;
    if (!sys::CreateImage(b_3, &id_b_3)) return false;
    if (!sys::CreateImage(b_4, &id_b_4)) return false;
    if (!sys::CreateImage(c_0, &id_c_0)) return false;
    if (!sys::CreateImage(c_1, &id_c_1)) return false;
    if (!sys::CreateImage(c_2, &id_c_2)) return false;
    if (!sys::CreateImage(c_3, &id_c_3)) return false;
    if (!sys::CreateImage(c_4, &id_c_4)) return false;
    return true;
  }
  bool Finalize() {
    sys::ReleaseImage(id_a_0);
    sys::ReleaseImage(id_a_1);
    sys::ReleaseImage(id_a_2);
    sys::ReleaseImage(id_a_3);
    sys::ReleaseImage(id_a_4);
    sys::ReleaseImage(id_b_0);
    sys::ReleaseImage(id_b_1);
    sys::ReleaseImage(id_b_2);
    sys::ReleaseImage(id_b_3);
    sys::ReleaseImage(id_b_4);
    sys::ReleaseImage(id_c_0);
    sys::ReleaseImage(id_c_1);
    sys::ReleaseImage(id_c_2);
    sys::ReleaseImage(id_c_3);
    sys::ReleaseImage(id_c_4);
    sys::ReleaseTexture(id_t);
    return true;
  }
  bool Paint() {
    Point2d pos_a(100.0, 200.0);
    Point2d pos_b(300.0, 200.0);
    Point2d pos_c(500.0, 200.0);
    const int delta = frame - last_frame;
    if (delta < 10) {
      sys::DrawImage(id_a_0, pos_a);
      sys::DrawImage(id_b_0, pos_b);
      sys::DrawImage(id_c_0, pos_c);
    } else if (delta < 20) {
      sys::DrawImage(id_a_1, pos_a);
      sys::DrawImage(id_b_1, pos_b);
      sys::DrawImage(id_c_1, pos_c);
    } else if (delta < 30) {
      sys::DrawImage(id_a_2, pos_a);
      sys::DrawImage(id_b_2, pos_b);
      sys::DrawImage(id_c_2, pos_c);
    } else if (delta < 40) {
      sys::DrawImage(id_a_3, pos_a);
      sys::DrawImage(id_b_3, pos_b);
      sys::DrawImage(id_c_3, pos_c);
    } else if (delta < 50) {
      sys::DrawImage(id_a_4, pos_a);
      sys::DrawImage(id_b_4, pos_b);
      sys::DrawImage(id_c_4, pos_c);
      if (delta == 49) last_frame = frame;
    } else {
      // No Impl.
    }
    ++frame;
    return true;
  }
};
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hinstance_previous,
                    LPTSTR cmdline, int cmdshow) {
  sys::SetWindowTitle(L"System test:picture");
  sys::SetWindowIcon(IDI_ICON1);
  sys::SetWindowSize(640, 480);
  sys::SetResolution(648, 480);
  sys::StartWithFullscreen(false);
  if (!sys::InitSystem(hinstance)) return -1;
  PictureTest test;
  if (!test.Init()) return -1;
  while (sys::UpdateSystem()) {
    sys::FillScreen(Color4b(0, 64, 64, 255));
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
