  // @file graphic_internal.h
  // @brief Declaration of graphic related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#ifndef GRAPHIC_INTERNAL_H_
#define GRAPHIC_INTERNAL_H_
#include <xnamath.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <unordered_map>
#include "./common.h"
#include "./common_internal.h"
#include "./graphic.h"
#include "shader/pshader1.h"  // Precompiler pixel shader
#include "shader/vshader1.h"  // Precompiler vertex shader
  //
  // These are internal macros related to graphic
  //
#define SYS_REFRESH_RATE_NUMERATOR    (60)  // Fixed.
#define SYS_REFRESH_RATE_DENOMINATOR  (1)  // Fixed.
#define SYS_MAXIMUM_FRAME_LATENCY     (1)  // 1 or 2 or 3
#define SYS_VERTEX_INPUT_NUM          (4)  // Fixed.
#define SYS_FONT_COLUMN_NUM           (16)  // Fixed.
#define SYS_FONT_ROW_NUM              (4)  // Fixed.
#define SYS_IMAGE_BUFFER_ELEM_NUM     (128)
#define SYS_TEXTURE_BUFFER_ELEM_NUM   (128)
#define SYS_FONT_BUFFER_ELEM_NUM      (4)
#define SYS_TEXT_BUF_SIZE             (128)

  //
  // These are internal enumerations and constants related to graphic
  //

namespace sys {
  //
  // These are internal structures related to graphic
  //
struct VSConstBufferData {  // Vertex shader
  XMMATRIX world_matrix;
};
struct PSConstBufferData {  // Pixel shader
  XMFLOAT4 source_color;
};
struct VertexInputData {
  float position[3];  // SV_POSITION
  float texcoord[2];  // TEXCOORD0
};
struct TextureData {
  int w;
  int h;
  float blend_factor[4];
  ID3D11ShaderResourceView* shader_resource_view[1];
  TextureData() :
      w(0),
      h(0),
      blend_factor(),
      shader_resource_view() { }
  void Release() {
    SYS_SAFE_RELEASE(shader_resource_view[0]);
  }
  bool IsNull() {
    return (shader_resource_view[0] == nullptr);
  }
};
struct ImageData {
  int w;
  int h;
  int texture_id;
  ID3D11Buffer* buffer;
  ImageData() :
      w(0),
      h(0),
      texture_id(0),
      buffer(nullptr) { }
  void Release() {
    SYS_SAFE_RELEASE(buffer);
  }
  bool IsNull() {
    return (buffer == nullptr);
  }
};
struct FontData {
  TextureData font_texture;
  ImageData font_image[SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM];
  FontData() :
    font_texture(),
    font_image() { }
  void Release() {
    for (int i = 0; i < SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM; ++i) {
      font_image[i].Release();
    }
    font_texture.Release();
  }
  bool IsNull() {
    return font_texture.IsNull();
  }
};
struct GraphicData {
  ID3D11Device* device;
  ID3D11DeviceContext* device_context;
  IDXGIDevice1* dxgi_device;
  IDXGIAdapter* dxgi_adaptor;
  IDXGIFactory* dxgi_factory;
  IDXGISwapChain* dxgi_swap_chain;
  ID3D11Texture2D* back_buffer;
  ID3D11RenderTargetView* render_target_view;
  ID3D11InputLayout* input_layout;
  ID3D11Buffer* vs_cbuffer;
  ID3D11Buffer* ps_cbuffer;
  ID3D11BlendState* blend_state;
  ID3D11SamplerState* sampler_state;
  ID3D11VertexShader* vertex_shader1;
  ID3D11PixelShader* pixel_shader1;
  //
  TextureData texture_buffer[SYS_TEXTURE_BUFFER_ELEM_NUM];
  ImageData image_buffer[SYS_IMAGE_BUFFER_ELEM_NUM];
  FontData font_buffer[SYS_FONT_BUFFER_ELEM_NUM];
  Vector2<int> resolution;
  bool on_fullscreen_start;
  bool on_power_save;
  std::unordered_map<wchar_t, int> font_map;
  GraphicData() :
      device(nullptr),
      device_context(nullptr),
      dxgi_device(nullptr),
      dxgi_adaptor(nullptr),
      dxgi_factory(nullptr),
      dxgi_swap_chain(nullptr),
      back_buffer(nullptr),
      render_target_view(nullptr),
      input_layout(nullptr),
      vs_cbuffer(nullptr),
      ps_cbuffer(nullptr),
      blend_state(nullptr),
      sampler_state(nullptr),
      vertex_shader1(nullptr),
      pixel_shader1(nullptr),
      resolution(640, 480),
      on_fullscreen_start(false),
      on_power_save(false) {
    const wchar_t font_array[SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM] =
      L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\] ";
    for (int i = 0; i < SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM; ++i) {
      font_map[font_array[i]] = i;
    }
  }
};

  //
  // These are internal functions related to graphic
  //
bool InitGraphic();
bool UpdateGraphic();
void FinalizeGraphic();
}
extern sys::GraphicData graphic_data;
#endif  // GRAPHIC_INTERNAL_H_
