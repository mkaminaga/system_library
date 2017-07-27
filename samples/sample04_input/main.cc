// @file main.h
// @brief Input test.
// @author Mamoru Kaminaga
// @date 2017-06-17 16:18:40
// Copyright 2017 Mamoru Kaminaga
#include "../../system.h"
#include "resource.h"
#include <string>
struct TextDrawer {
  int id_a;
  sys::FontDesc a;
  TextDrawer() {
    a.resource_desc.use_mem = false;
    a.resource_desc.file_name = L"font.png";
    a.s = 1.4;
  }
  bool Init() {
    if (!sys::CreateFont(a, &id_a)) return false;
    return true;
  }
  bool Finalize() {
    sys::ReleaseFont(id_a);
    return true;
  }
  bool DrawVirtualInputAssign() {
    sys::DrawText(id_a, Point2d(10.0, 10.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT,
                  L"V=VIRTUAL INPUT,K=KEYBOARD,J=JOYPAD");
    sys::DrawText(id_a, Point2d(10.0, 30.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:DOWN K:DOWN J:STICK DOWN");
    sys::DrawText(id_a, Point2d(10.0, 50.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:LEFT K:LEFT J:STICK LEFT");
    sys::DrawText(id_a, Point2d(10.0, 70.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:RIGHT K:RIGHT J:STICK RIGHT");
    sys::DrawText(id_a, Point2d(10.0, 90.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:UP K:UP J:STICK UP");
    sys::DrawText(id_a, Point2d(10.0, 110.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:A K:A J:1");
    sys::DrawText(id_a, Point2d(10.0, 130.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:B K:B J:2");
    sys::DrawText(id_a, Point2d(10.0, 150.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:C K:C J:3");
    sys::DrawText(id_a, Point2d(10.0, 170.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:D K:D J:4");
    sys::DrawText(id_a, Point2d(10.0, 190.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:E K:E J:5");
    sys::DrawText(id_a, Point2d(10.0, 210.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:F K:F J:6");
    sys::DrawText(id_a, Point2d(10.0, 230.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:G K:G J:7");
    sys::DrawText(id_a, Point2d(10.0, 250.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:H K:H J:8");
    sys::DrawText(id_a, Point2d(10.0, 270.0), Color4b(0, 0, 0, 0),
                  SYS_FONTMODE_TOP_LEFT, L"V:I K:I J:9");
    return true;
  }
  bool DrawVirtualInputPushed(const std::wstring& key_str) {
    return sys::DrawText(id_a, Point2d(10.0, 300.0), Color4b(0, 0, 0, 0),
                         SYS_FONTMODE_TOP_LEFT, L"%s IS PUSHED",
                         key_str.c_str());
  }
  bool DrawVirtualInputPressed(const std::wstring& key_str) {
    return sys::DrawText(id_a, Point2d(10.0, 320.0), Color4b(0, 0, 0, 0),
                         SYS_FONTMODE_TOP_LEFT, L"%s IS PRESSED",
                         key_str.c_str());
  }
};
struct InputTest {
  TextDrawer drawer;
  InputTest() : drawer() { }
  bool Init() {
    sys::SetJoypadThreshold(50, 50);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_A, SYS_KEY_A, SYS_JOYPAD_KEY_0);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_B, SYS_KEY_B, SYS_JOYPAD_KEY_1);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_C, SYS_KEY_C, SYS_JOYPAD_KEY_2);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_D, SYS_KEY_D, SYS_JOYPAD_KEY_3);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_E, SYS_KEY_E, SYS_JOYPAD_KEY_4);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_F, SYS_KEY_F, SYS_JOYPAD_KEY_5);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_G, SYS_KEY_G, SYS_JOYPAD_KEY_6);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_H, SYS_KEY_H, SYS_JOYPAD_KEY_7);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_I, SYS_KEY_I, SYS_JOYPAD_KEY_8);
    sys::SetVirtualInputKey(SYS_VIRTUAL_KEY_J, SYS_KEY_J, SYS_JOYPAD_KEY_9);
    if (!drawer.Init()) return false;
    return true;
  }
  bool Finalize() {
    drawer.Finalize();
    return true;
  }
  std::wstring KeyToString(SYS_VIRTUAL_KEY virtual_key) {
    switch (virtual_key) {
      case SYS_VIRTUAL_KEY_DOWN: return L"SYS_VIRTUAL_KEY_DOWN";
      case SYS_VIRTUAL_KEY_LEFT: return L"SYS_VIRTUAL_KEY_LEFT";
      case SYS_VIRTUAL_KEY_RIGHT: return L"SYS_VIRTUAL_KEY_RIGHT";
      case SYS_VIRTUAL_KEY_UP: return L"SYS_VIRTUAL_KEY_UP";
      case SYS_VIRTUAL_KEY_A: return L"SYS_VIRTUAL_KEY_A";
      case SYS_VIRTUAL_KEY_B: return L"SYS_VIRTUAL_KEY_B";
      case SYS_VIRTUAL_KEY_C: return L"SYS_VIRTUAL_KEY_C";
      case SYS_VIRTUAL_KEY_D: return L"SYS_VIRTUAL_KEY_D";
      case SYS_VIRTUAL_KEY_E: return L"SYS_VIRTUAL_KEY_E";
      case SYS_VIRTUAL_KEY_F: return L"SYS_VIRTUAL_KEY_F";
      case SYS_VIRTUAL_KEY_G: return L"SYS_VIRTUAL_KEY_G";
      case SYS_VIRTUAL_KEY_H: return L"SYS_VIRTUAL_KEY_H";
      case SYS_VIRTUAL_KEY_I: return L"SYS_VIRTUAL_KEY_I";
      case SYS_VIRTUAL_KEY_J: return L"SYS_VIRTUAL_KEY_J";
      default: return L"?";
    }
  }
  bool Paint() {
    drawer.DrawVirtualInputAssign();
    SYS_VIRTUAL_KEY keys[] = {
      SYS_VIRTUAL_KEY_DOWN,
      SYS_VIRTUAL_KEY_LEFT,
      SYS_VIRTUAL_KEY_RIGHT,
      SYS_VIRTUAL_KEY_UP,
      SYS_VIRTUAL_KEY_A,
      SYS_VIRTUAL_KEY_B,
      SYS_VIRTUAL_KEY_C,
      SYS_VIRTUAL_KEY_D,
      SYS_VIRTUAL_KEY_E,
      SYS_VIRTUAL_KEY_F,
      SYS_VIRTUAL_KEY_G,
      SYS_VIRTUAL_KEY_H,
      SYS_VIRTUAL_KEY_I,
      SYS_VIRTUAL_KEY_J,
    };
    std::wstring key_str;
    for (int i = 0; i < ARRAYSIZE(keys); i++) {
      if (sys::GetVirtualInputStatus(keys[i])) {
        key_str = KeyToString(keys[i]);
        drawer.DrawVirtualInputPushed(key_str.c_str());
      }
      if (sys::GetVirtualInputPressed(keys[i])) {
        key_str = KeyToString(keys[i]);
        drawer.DrawVirtualInputPressed(key_str.c_str());
      }
    }
    return true;
  }
};
int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hinstance_previous,
                    LPTSTR cmdline, int cmdshow) {
  sys::SetWindowTitle(L"System test:input");
  sys::SetWindowIcon(IDI_ICON1);
  sys::SetWindowSize(640, 480);
  sys::SetResolution(648, 480);
  sys::StartWithFullscreen(false);
  if (!sys::InitSystem(hinstance)) return -1;
  InputTest test;
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
