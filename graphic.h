  // @file graphic
  // @brief Declaration of graphic related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef GRAPHIC_H_
#define GRAPHIC_H_
#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include <windows.h>
#include <Vecmath.h>
#include <string>
#include "./common.h"
  //
  // These are public macros related to graphic
  //
#define SYS_ERROR_DUPLICATE_TEXTURE_ID    L"Error! Duplicate texture id:%d"
#define SYS_ERROR_DUPLICATE_FONT_ID       L"Error! Duplicate font id:%d"
#define SYS_ERROR_DUPLICATE_IMAGE_ID      L"Error! Duplicate image id:%d"
#define SYS_ERROR_NULL_TEXTURE_ID         L"Error! Null texture id:%d"
#define SYS_ERROR_NULL_IMAGE_ID           L"Error! Null image id:%d"
#define SYS_ERROR_NULL_FONT_ID            L"Error! Null font id:%d"
#define SYS_ERROR_INVALID_TEXTURE_ID      L"Error! Invalid texture id:%d"
#define SYS_ERROR_INVALID_IMAGE_ID        L"Error! Invalid image id:%d"
#define SYS_ERROR_INVALID_FONT_ID         L"Error! Invalid font id:%d"
#define SYS_ERROR_DXGI_DEVICE_REMOVED     L"Error! DXGI_ERROR_DEVICE_REMOVED"
#define SYS_ERROR_DEVICE_HUNG             L"Error! DXGI_ERROR_DEVICE_HUNG"
#define SYS_ERROR_DEVICE_RESET            L"Error! DXGI_ERROR_DEVICE_RESET"
#define SYS_ERROR_DEVICE_REMOVED          L"Error! DXGI_ERROR_DEVICE_REMOVED"
#define SYS_ERROR_DRIVER_INTERNAL_ERROR   L"Error! DXGI_ERROR_DRIVER_INTERNAL_ERROR"  // NOLINT
#define SYS_ERROR_INVALID_CALL            L"Error! DXGI_ERROR_INVALID_CALL"

  //
  // These are public enumerations and constants related to graphic
  //
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

namespace sys {
  //
  // These are public structures related to graphic
  //
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

  //
  // These are public functions related to graphic
  //
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
}
#endif  // GRAPHIC_H_
