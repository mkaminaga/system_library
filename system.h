// @file system.h
// @brief API for game development
// @author Mamoru kaminaga
// @date 2017-06-13 20:21:57
// Copyright 2017 Mamoru Kaminaga
#ifndef SYSTEM_SYSTEM_H_
#define SYSTEM_SYSTEM_H_
#include <dinput.h>
#include <wchar.h>
#include <windows.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <Vecmath.h>
#include <string>
#define SYS_ELEMNUM_BUTTON_KEY        (14)
#define SYS_ELEMNUM_KEYID             (256)
#define SYS_ELEMNUM_CONTROLLERID      (36)
enum SYS_IMAGEMODE {
  SYS_IMAGEMODE_DEFAULT,
  SYS_IMAGEMODE_HINVERT,
  SYS_IMAGEMODE_VINVERT,
  SYS_IMAGEMODE_ROT90,
  SYS_IMAGEMODE_ROT180,
  SYS_IMAGEMODE_ROT270,
};
enum SYS_FONTMODE {
  SYS_FONTMODE_TOP_LEFT,
  SYS_FONTMODE_TOP_CENTER,
  SYS_FONTMODE_TOP_RIGHT,
  SYS_FONTMODE_MIDDLE_LEFT,
  SYS_FONTMODE_MIDDLE_CENTER,
  SYS_FONTMODE_MIDDLE_RIGHT,
  SYS_FONTMODE_BOTTOM_LEFT,
  SYS_FONTMODE_BOTTOM_CENTER,
  SYS_FONTMODE_BOTTOM_RIGHT,
};
enum SYS_VIRTUAL_KEY {
  SYS_VIRTUAL_KEY_DOWN = 0,  // Fixed
  SYS_VIRTUAL_KEY_LEFT = 1,  // Fixed
  SYS_VIRTUAL_KEY_RIGHT = 2,  // Fixed
  SYS_VIRTUAL_KEY_UP = 3,  // Fixed
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
enum SYS_KEY {
  SYS_KEY_ESCAPE       = DIK_ESCAPE,
  SYS_KEY_1            = DIK_1,
  SYS_KEY_2            = DIK_2,
  SYS_KEY_3            = DIK_3,
  SYS_KEY_4            = DIK_4,
  SYS_KEY_5            = DIK_5,
  SYS_KEY_6            = DIK_6,
  SYS_KEY_7            = DIK_7,
  SYS_KEY_8            = DIK_8,
  SYS_KEY_9            = DIK_9,
  SYS_KEY_0            = DIK_0,
  SYS_KEY_MINUS        = DIK_MINUS,
  SYS_KEY_EQUALS       = DIK_EQUALS,
  SYS_KEY_BACK         = DIK_BACK,
  SYS_KEY_TAB          = DIK_TAB,
  SYS_KEY_Q            = DIK_Q,
  SYS_KEY_W            = DIK_W,
  SYS_KEY_E            = DIK_E,
  SYS_KEY_R            = DIK_R,
  SYS_KEY_T            = DIK_T,
  SYS_KEY_Y            = DIK_Y,
  SYS_KEY_U            = DIK_U,
  SYS_KEY_I            = DIK_I,
  SYS_KEY_O            = DIK_O,
  SYS_KEY_P            = DIK_P,
  SYS_KEY_LBRACKET     = DIK_LBRACKET,
  SYS_KEY_RBRACKET     = DIK_RBRACKET,
  SYS_KEY_RETURN       = DIK_RETURN,
  SYS_KEY_LCONTROL     = DIK_LCONTROL,
  SYS_KEY_A            = DIK_A,
  SYS_KEY_S            = DIK_S,
  SYS_KEY_D            = DIK_D,
  SYS_KEY_F            = DIK_F,
  SYS_KEY_G            = DIK_G,
  SYS_KEY_H            = DIK_H,
  SYS_KEY_J            = DIK_J,
  SYS_KEY_K            = DIK_K,
  SYS_KEY_L            = DIK_L,
  SYS_KEY_SEMICOLON    = DIK_SEMICOLON,
  SYS_KEY_APOSTROPHE   = DIK_APOSTROPHE,
  SYS_KEY_GRAVE        = DIK_GRAVE,
  SYS_KEY_LSHIFT       = DIK_LSHIFT,
  SYS_KEY_BACKSLASH    = DIK_BACKSLASH,
  SYS_KEY_Z            = DIK_Z,
  SYS_KEY_X            = DIK_X,
  SYS_KEY_C            = DIK_C,
  SYS_KEY_V            = DIK_V,
  SYS_KEY_B            = DIK_B,
  SYS_KEY_N            = DIK_N,
  SYS_KEY_M            = DIK_M,
  SYS_KEY_COMMA        = DIK_COMMA,
  SYS_KEY_PERIOD       = DIK_PERIOD,
  SYS_KEY_SLASH        = DIK_SLASH,
  SYS_KEY_RSHIFT       = DIK_RSHIFT,
  SYS_KEY_MULTIPLY     = DIK_MULTIPLY,
  SYS_KEY_LMENU        = DIK_LMENU,
  SYS_KEY_SPACE        = DIK_SPACE,
  SYS_KEY_CAPITAL      = DIK_CAPITAL,
  SYS_KEY_F1           = DIK_F1,
  SYS_KEY_F2           = DIK_F2,
  SYS_KEY_F3           = DIK_F3,
  SYS_KEY_F4           = DIK_F4,
  SYS_KEY_F5           = DIK_F5,
  SYS_KEY_F6           = DIK_F6,
  SYS_KEY_F7           = DIK_F7,
  SYS_KEY_F8           = DIK_F8,
  SYS_KEY_F9           = DIK_F9,
  SYS_KEY_F10          = DIK_F10,
  SYS_KEY_KEYLOCK      = DIK_NUMLOCK,
  SYS_KEY_SCROLL       = DIK_SCROLL,
  SYS_KEY_KEYPAD7      = DIK_NUMPAD7,
  SYS_KEY_KEYPAD8      = DIK_NUMPAD8,
  SYS_KEY_KEYPAD9      = DIK_NUMPAD9,
  SYS_KEY_SUBTRACT     = DIK_SUBTRACT,
  SYS_KEY_KEYPAD4      = DIK_NUMPAD4,
  SYS_KEY_KEYPAD5      = DIK_NUMPAD5,
  SYS_KEY_KEYPAD6      = DIK_NUMPAD6,
  SYS_KEY_ADD          = DIK_ADD,
  SYS_KEY_KEYPAD1      = DIK_NUMPAD1,
  SYS_KEY_KEYPAD2      = DIK_NUMPAD2,
  SYS_KEY_KEYPAD3      = DIK_NUMPAD3,
  SYS_KEY_KEYPAD0      = DIK_NUMPAD0,
  SYS_KEY_DECIMAL      = DIK_DECIMAL,
  SYS_KEY_OEM_102      = DIK_OEM_102,
  SYS_KEY_F11          = DIK_F11,
  SYS_KEY_F12          = DIK_F12,
  SYS_KEY_F13          = DIK_F13,
  SYS_KEY_F14          = DIK_F14,
  SYS_KEY_F15          = DIK_F15,
  SYS_KEY_KANA         = DIK_KANA,
  SYS_KEY_ABNT_C1      = DIK_ABNT_C1,
  SYS_KEY_CONVERT      = DIK_CONVERT,
  SYS_KEY_NOCONVERT    = DIK_NOCONVERT,
  SYS_KEY_YEN          = DIK_YEN,
  SYS_KEY_ABNT_C2      = DIK_ABNT_C2,
  SYS_KEY_KEYPADEQUALS = DIK_NUMPADEQUALS,
  SYS_KEY_PREVTRACK    = DIK_PREVTRACK,
  SYS_KEY_AT           = DIK_AT,
  SYS_KEY_COLON        = DIK_COLON,
  SYS_KEY_UNDERLINE    = DIK_UNDERLINE,
  SYS_KEY_KANJI        = DIK_KANJI,
  SYS_KEY_STOP         = DIK_STOP,
  SYS_KEY_AX           = DIK_AX,
  SYS_KEY_UNLABELED    = DIK_UNLABELED,
  SYS_KEY_NEXTTRACK    = DIK_NEXTTRACK,
  SYS_KEY_KEYPADENTER  = DIK_NUMPADENTER,
  SYS_KEY_RCONTROL     = DIK_RCONTROL,
  SYS_KEY_MUTE         = DIK_MUTE,
  SYS_KEY_CALCULATOR   = DIK_CALCULATOR,
  SYS_KEY_PLAYPAUSE    = DIK_PLAYPAUSE,
  SYS_KEY_MEDIASTOP    = DIK_MEDIASTOP,
  SYS_KEY_VOLUMEDOWN   = DIK_VOLUMEDOWN,
  SYS_KEY_VOLUMEUP     = DIK_VOLUMEUP,
  SYS_KEY_WEBHOME      = DIK_WEBHOME,
  SYS_KEY_KEYPADCOMMA  = DIK_NUMPADCOMMA,
  SYS_KEY_DIVIDE       = DIK_DIVIDE,
  SYS_KEY_SYSRQ        = DIK_SYSRQ,
  SYS_KEY_RMENU        = DIK_RMENU,
  SYS_KEY_PAUSE        = DIK_PAUSE,
  SYS_KEY_HOME         = DIK_HOME,
  SYS_KEY_UP           = DIK_UP,
  SYS_KEY_PRIOR        = DIK_PRIOR,
  SYS_KEY_LEFT         = DIK_LEFT,
  SYS_KEY_RIGHT        = DIK_RIGHT,
  SYS_KEY_END          = DIK_END,
  SYS_KEY_DOWN         = DIK_DOWN,
  SYS_KEY_NEXT         = DIK_NEXT,
  SYS_KEY_INSERT       = DIK_INSERT,
  SYS_KEY_DELETE       = DIK_DELETE,
  SYS_KEY_LWIN         = DIK_LWIN,
  SYS_KEY_RWIN         = DIK_RWIN,
  SYS_KEY_APPS         = DIK_APPS,
  SYS_KEY_POWER        = DIK_POWER,
  SYS_KEY_SLEEP        = DIK_SLEEP,
  SYS_KEY_WAKE         = DIK_WAKE,
  SYS_KEY_WEBSEARCH    = DIK_WEBSEARCH,
  SYS_KEY_WEBFAVORITES = DIK_WEBFAVORITES,
  SYS_KEY_WEBREFRESH   = DIK_WEBREFRESH,
  SYS_KEY_WEBSTOP      = DIK_WEBSTOP,
  SYS_KEY_WEBFORWARD   = DIK_WEBFORWARD,
  SYS_KEY_WEBBACK      = DIK_WEBBACK,
  SYS_KEY_MYCOMPUTER   = DIK_MYCOMPUTER,
  SYS_KEY_MAIL         = DIK_MAIL,
  SYS_KEY_MEDIASELECT  = DIK_MEDIASELECT,
};
enum SYS_JOYPAD_KEY {
  SYS_JOYPAD_KEY_0 = 0,
  SYS_JOYPAD_KEY_1,
  SYS_JOYPAD_KEY_2,
  SYS_JOYPAD_KEY_3,
  SYS_JOYPAD_KEY_4,
  SYS_JOYPAD_KEY_5,
  SYS_JOYPAD_KEY_6,
  SYS_JOYPAD_KEY_7,
  SYS_JOYPAD_KEY_8,
  SYS_JOYPAD_KEY_9,
  SYS_JOYPAD_KEY_10,
  SYS_JOYPAD_KEY_11,
  SYS_JOYPAD_KEY_12,
  SYS_JOYPAD_KEY_13,
  SYS_JOYPAD_KEY_14,
  SYS_JOYPAD_KEY_15,
  SYS_JOYPAD_KEY_16,
  SYS_JOYPAD_KEY_17,
  SYS_JOYPAD_KEY_18,
  SYS_JOYPAD_KEY_19,
  SYS_JOYPAD_KEY_20,
  SYS_JOYPAD_KEY_21,
  SYS_JOYPAD_KEY_22,
  SYS_JOYPAD_KEY_23,
  SYS_JOYPAD_KEY_24,
  SYS_JOYPAD_KEY_25,
  SYS_JOYPAD_KEY_26,
  SYS_JOYPAD_KEY_27,
  SYS_JOYPAD_KEY_28,
  SYS_JOYPAD_KEY_29,
  SYS_JOYPAD_KEY_30,
  SYS_JOYPAD_KEY_31,
  SYS_JOYPAD_KEY_DOWN,
  SYS_JOYPAD_KEY_LEFT,
  SYS_JOYPAD_KEY_RIGHT,
  SYS_JOYPAD_KEY_UP,
};
namespace sys {
struct ResourceDesc {
  std::wstring file_name;
  uint8_t* mem_ptr;
  size_t mem_size;
  bool use_mem;
  ResourceDesc() :
      file_name(),
      mem_ptr(nullptr),
      mem_size(0),
      use_mem(false) { }
};
////////////////////////////////////////
// System
////////////////////////////////////////
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
////////////////////////////////////////
// Graphics
////////////////////////////////////////
struct TextureDesc {
  ResourceDesc resource_desc;
  TextureDesc() : resource_desc() { }
};
struct ImageDesc {
  int texture_id;
  int x;
  int y;
  int w;
  int h;
  double s;
  SYS_IMAGEMODE image_mode;
  ImageDesc() :
      texture_id(0),
      x(0),
      y(0),
      w(0),
      h(0),
      s(1.0),
      image_mode(SYS_IMAGEMODE_DEFAULT) { }
};
struct FontDesc {
  ResourceDesc resource_desc;
  double s;
  SYS_IMAGEMODE image_mode;
  FontDesc() :
      resource_desc(),
      s(1.0),
      image_mode(SYS_IMAGEMODE_DEFAULT) { }
};
void SetResolution(int x, int y);
void StartWithFullscreen(bool fullscreen);
bool FillScreen(const Color4b& color);
bool CreateTexture(const TextureDesc& desc, int texture_id);
bool ReleaseTexture(int texture_id);
bool GetTextureSize(int texture_id, Vector2d* size);
bool CreateImage(const ImageDesc& desc, int image_id);
bool ReleaseImage(int image_id);
bool GetImageSize(int image_id, Vector2d* size);
bool DrawImage(int image_id, const Vector2d& position, const Color4b& color);
bool CreateFont(const FontDesc& desc, int font_id);
bool ReleaseFont(int font_id);
bool GetFontSize(int font_id, Vector2d* size);
bool GetTextSize(int font_id, Vector2d* size, const wchar_t* format, ...);
bool DrawText(int font_id, const Vector2d& position, const Color4b& color,
              SYS_FONTMODE font_mode, const wchar_t* format, ...);
////////////////////////////////////////
// Sound
////////////////////////////////////////
struct WaveDesc {
  ResourceDesc resource_desc;
  WaveDesc() : resource_desc() { }
};
struct StreamingDesc {
  ResourceDesc resource_desc;
  bool use_loop;
  StreamingDesc() :
    resource_desc(),
    use_loop(false) { }
};
bool CreateWave(const WaveDesc& desc, int wave_id);
bool ReleaseWave(int wave_id);
bool PlayWave(int wave_id);
bool StopWave(int wave_id);
bool PlayStreaming(const StreamingDesc& desc);
bool PauseStreaming();
bool ContinueStreaming();
bool StopStreaming();
////////////////////////////////////////
// Input
////////////////////////////////////////
bool SetVirtualInputKey(SYS_VIRTUAL_KEY virtual_key, SYS_KEY key,
                        SYS_JOYPAD_KEY joypad_key);
void SetJoypadThreshold(int threshold_x, int threshold_y);
bool GetKeyboardStatus(SYS_KEY key);
bool GetJoypadStatus(SYS_JOYPAD_KEY joypad_key);
bool GetVirtualInputStatus(SYS_VIRTUAL_KEY virtual_key);
bool GetVirtualInputPressed(SYS_VIRTUAL_KEY virtual_key);
////////////////////////////////////////
// Debug
////////////////////////////////////////
bool ErrorDialogBox(const wchar_t* format, ...);
}  // namespace sys
#endif  // SYSTEM_SYSTEM_H_
