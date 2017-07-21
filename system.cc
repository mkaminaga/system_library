// @file system.cc
// @brief API for game development
// @author Mamoru kaminaga
// @date 2017-02-16 15:55:30
// Copyright 2017 Mamoru Kaminaga
#include "system.h"
#include <assert.h>
#include <wchar.h>
#include <windows.h>
#include <string>
// Graphic error
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
#define SYS_ERROR_DRIVER_INTERNAL_ERROR   L"Error! DXGI_ERROR_DRIVER_INTERNAL_ERROR"
#define SYS_ERROR_INVALID_CALL            L"Error! DXGI_ERROR_INVALID_CALL"
// Sound error
#define SYS_ERROR_DUPLICATE_WAVE_ID       L"Error! Duplicate wave id:%d"
#define SYS_ERROR_NULL_WAVE_ID            L"Error! Null wave id:%d"
#define SYS_ERROR_INVALID_WAVE_ID         L"Error! Invalid wave id:%d"
// Input error
#define SYS_ERROR_INVALID_KEY             L"Error! Invalid key:%d"
#define SYS_ERROR_INVALID_JOYPAD_KEY      L"Error! Invalid joypad key:%d"
#define SYS_ERROR_INVALID_VIRTUAL_KEY     L"Error! Invalid virtual key:%d"
#define SYS_ERROR_INVALID_THRESHOLD       L"Error! Invalid threshold:%d"
#define SYS_ERROR_NO_INPUT_DEVICE         L"Error! No input device detected"
#define SYS_ERROR_ARROW_KEYS_NOT_CUSTOM   L"Error! Allow keys are not custom"
// Macro functions.
#define SYS_SAFE_RELEASE(p) if ((p) != nullptr) {(p)->Release(); (p)=nullptr;}
////////////////////////////////////////
// Graphics
////////////////////////////////////////
#include <xnamath.h>
#include <d3d11.h>
#include <d3dx11.h>
////////////////////////////////////////
// Sound
////////////////////////////////////////
#include <dsound.h>
#include <process.h>
////////////////////////////////////////
// Input
////////////////////////////////////////
#include <dinput.h>
////////////////////////////////////////
// System
////////////////////////////////////////
#include <ShellScalingAPI.h>
#include <mmsystem.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <Vecmath.h>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include "shader/pshader1.h"  // Precompiler pixel shader
#include "shader/vshader1.h"  // Precompiler vertex shader
namespace sys {
namespace {
////////////////////////////////////////
// System
////////////////////////////////////////
#define SYS_WINDOW_STYLE    (WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX)
#define SYS_WM_APP_STOP     (WM_APP + 1)
#define SYS_FPS_SAMPLE_NUM  (4)
struct SystemData {
  HINSTANCE hinstance;
  HWND hwnd;
  Vector2<int> window_size;
  std::wstring window_title;
  int icon_id;
  int fps_last_ms;
  int fps_timer_id;
  int last_ms;
  int ms;
  int one_loop_ms;
  int timer_int_ms;
  double fps;
  bool window_forcus;
  std::list<double> fps_sample;
  SystemData() :
      hinstance(nullptr),
      hwnd(nullptr),
      window_size(640, 480),
      window_title(L"System"),
      icon_id(0),
      fps_last_ms(0),
      ms(0),
      one_loop_ms(0),
      timer_int_ms(0),
      fps(0.0),
      window_forcus(false) { }
};
SystemData system_data;
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
  system_data.one_loop_ms = 16 / system_data.timer_int_ms;
  return true;
}
void FinalizeTimerPeriod() {
  timeEndPeriod(system_data.timer_int_ms);
}
void CALLBACK TimerProc(UINT timer_id, UINT msg, DWORD_PTR user,
                        DWORD_PTR ptr1, DWORD_PTR ptr2) {
  ++system_data.ms;
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
  MSG msg;
  while ((system_data.ms - system_data.last_ms) <= system_data.one_loop_ms) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT) return false;
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  system_data.last_ms = system_data.ms;
  return true;
}
bool InitFPSCnt() {
  for (int i = SYS_FPS_SAMPLE_NUM; i; --i) {
    system_data.fps_sample.push_back(0);
  }
  return true;
}
void FinalizeFPSCnt() { /* No Impl */ }
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
////////////////////////////////////////
// Graphics
////////////////////////////////////////
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
struct VSConstBufferData {  // Vertex Shader
  XMMATRIX world_matrix;
};
struct PSConstBufferData {  // Pxcel Shader
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
      on_power_save(false) { }
};
GraphicData graphic_data;
//
bool CreateD3DDevice() {
  const D3D_DRIVER_TYPE driver_types[] = {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };
  const D3D_FEATURE_LEVEL feature_levels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1,
  };
  for (int i = 0; i < ARRAYSIZE(driver_types); ++i) {
    if (SUCCEEDED(
          D3D11CreateDevice(
            nullptr,
            driver_types[i],
            nullptr,
            0,
            feature_levels,
            ARRAYSIZE(feature_levels),
            D3D11_SDK_VERSION,
            &graphic_data.device,
            nullptr,
            &graphic_data.device_context))) {
      return true;
    }
  }
  return false;
}
bool CreateDXGIDevice() {
  if (FAILED(
        graphic_data.device->QueryInterface(
          __uuidof(IDXGIDevice1),
          reinterpret_cast<void**>(&graphic_data.dxgi_device)))) {
    return false;
  }
  if (FAILED(
        graphic_data.dxgi_device->GetAdapter(
          &graphic_data.dxgi_adaptor))) {
    return false;
  }
  graphic_data.dxgi_adaptor->GetParent(
      __uuidof(IDXGIFactory),
      reinterpret_cast<void**>(&graphic_data.dxgi_factory));
  if (graphic_data.dxgi_factory == nullptr) return false;
  return true;
}
bool CreateSwapChain() {
  DXGI_SWAP_CHAIN_DESC desc;
  desc.BufferDesc.Width = graphic_data.resolution.x;
  desc.BufferDesc.Height = graphic_data.resolution.y;
  desc.BufferDesc.RefreshRate.Numerator = SYS_REFRESH_RATE_NUMERATOR;
  desc.BufferDesc.RefreshRate.Denominator = SYS_REFRESH_RATE_DENOMINATOR;
  desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  desc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
  desc.SampleDesc.Count = 1;
  desc.SampleDesc.Quality = 0;
  desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  desc.BufferCount = 1;
  desc.OutputWindow = system_data.hwnd;
  desc.Windowed = !graphic_data.on_fullscreen_start;
  desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
  desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  if (FAILED(
        graphic_data.dxgi_factory->CreateSwapChain(
          graphic_data.device,
          &desc,
          &graphic_data.dxgi_swap_chain))) {
    return false;
  }
  return true;
}
bool CreateBackBuffer() {
  if (FAILED(
        graphic_data.dxgi_swap_chain->GetBuffer(
          0,
          __uuidof(ID3D11Texture2D),
          reinterpret_cast<void**>(&graphic_data.back_buffer)))) {
    return false;
  }
  return true;
}
bool CreateRenderTargetView() {
  if (FAILED(
        graphic_data.device->CreateRenderTargetView(
          graphic_data.back_buffer,
          nullptr,
          &graphic_data.render_target_view))) {
    return false;
  }
  return true;
}
bool SetViewPort() {
  D3D11_VIEWPORT vp;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  vp.Width = static_cast<float>(graphic_data.resolution.x);
  vp.Height = static_cast<float>(graphic_data.resolution.y);
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  graphic_data.device_context->RSSetViewports(1, &vp);
  return true;
}
bool CreateVSConstBuffer() {
  D3D11_BUFFER_DESC desc;
  desc.ByteWidth = sizeof(VSConstBufferData) * 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.StructureByteStride = sizeof(VSConstBufferData);
  if (FAILED(
        graphic_data.device->CreateBuffer(
          &desc,
          nullptr,
          &graphic_data.vs_cbuffer))) {
    return false;
  }
  return true;
}
bool CreatePSConstBuffer() {
  D3D11_BUFFER_DESC desc;
  desc.ByteWidth = sizeof(PSConstBufferData) * 1;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  desc.CPUAccessFlags = 0;
  desc.MiscFlags = 0;
  desc.StructureByteStride = sizeof(PSConstBufferData);
  if (FAILED(
        graphic_data.device->CreateBuffer(
          &desc,
          nullptr,
          &graphic_data.ps_cbuffer))) {
    return false;
  }
  return true;
}
int CreateBlendState() {
  D3D11_BLEND_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.AlphaToCoverageEnable = FALSE;
  desc.IndependentBlendEnable = FALSE;
  //
  desc.RenderTarget[0].BlendEnable = TRUE;
  desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  if (FAILED(
        graphic_data.device->CreateBlendState(
          &desc,
          &graphic_data.blend_state))) {
    return false;
  }
  return true;
}
bool CreateSamplerState() {
  D3D11_SAMPLER_DESC desc;
  memset(&desc, 0, sizeof(desc));
  desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  desc.MipLODBias = 0;
  desc.MaxAnisotropy = 16;
  desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  desc.MinLOD = -D3D11_FLOAT32_MAX;
  desc.MaxLOD = D3D11_FLOAT32_MAX;
  if (FAILED(
        graphic_data.device->CreateSamplerState(
          &desc,
          &graphic_data.sampler_state))) {
    return false;
  }
  return true;
}
bool CreateVertexInputLayout(const BYTE** asmptr, size_t asm_size) {
  D3D11_INPUT_ELEMENT_DESC desc[2];  // Description of VertexInputData
  desc[0].SemanticName = "POSITION";
  desc[0].SemanticIndex = 0;
  desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  desc[0].InputSlot = 0;
  desc[0].AlignedByteOffset = 0;
  desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  desc[0].InstanceDataStepRate = 0;
  //
  desc[1].SemanticName = "TEXCOORD";
  desc[1].SemanticIndex = 0;
  desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
  desc[1].InputSlot = 0;
  desc[1].AlignedByteOffset = 4 * 3;
  desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  desc[1].InstanceDataStepRate = 0;
  //
  if (FAILED(
        graphic_data.device->CreateInputLayout(
          desc,
          ARRAYSIZE(desc),
          asmptr,
          asm_size,
          &graphic_data.input_layout))) {
    return false;
  }
  return true;
}
bool CreateVertexShader(const BYTE** asmptr, size_t asm_size,
                       ID3D11VertexShader** shader) {
  if (FAILED(
        graphic_data.device->CreateVertexShader(
          asmptr,
          asm_size,
          nullptr,
          shader))) {
    return false;
  }
  return true;
}
bool CreatePixelShader(const BYTE** asmptr, size_t asm_size,
                       ID3D11PixelShader** shader) {
  if (FAILED(
        graphic_data.device->CreatePixelShader(
          asmptr,
          asm_size,
          nullptr,
          shader))) {
    return false;
  }
  return true;
}
bool CreateGraphicPipeline() {
  graphic_data.device_context->IASetPrimitiveTopology(
      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  graphic_data.device_context->IASetInputLayout(
      graphic_data.input_layout);
  graphic_data.device_context->VSSetShader(
      graphic_data.vertex_shader1,
      nullptr,
      0);
  graphic_data.device_context->PSSetShader(
      graphic_data.pixel_shader1,
      nullptr,
      0);
  graphic_data.device_context->PSSetSamplers(
      0,
      1,
      &graphic_data.sampler_state);
  graphic_data.device_context->OMSetRenderTargets(
      1,
      &graphic_data.render_target_view,
      nullptr);
  return true;
}
bool EnableWindowModeChange() {
  if (FAILED(
        graphic_data.dxgi_factory->MakeWindowAssociation(
          system_data.hwnd,
          0))) {
    return false;
  }
  return true;
}
bool SetMaximumFrameLatency() {
  if (FAILED(
        graphic_data.dxgi_device->SetMaximumFrameLatency(
          SYS_MAXIMUM_FRAME_LATENCY))) {
    return false;
  }
  return true;
}
bool GetFullscreenSize(Vector2<UINT>* display_size) {
  BOOL not_used = 0;
  IDXGIOutput* dxgi_output;
  graphic_data.dxgi_swap_chain->GetFullscreenState(&not_used, &dxgi_output);
  if (!dxgi_output) return false;
  // Check list item num
  UINT item_num = 0;
  DXGI_MODE_DESC desc;
  desc.RefreshRate.Numerator = SYS_REFRESH_RATE_NUMERATOR;
  desc.RefreshRate.Denominator = SYS_REFRESH_RATE_DENOMINATOR;
  desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  if (FAILED(
        dxgi_output->GetDisplayModeList(
          desc.Format,
          0,
          &item_num,
          0))) {
    return false;
  }
  if (item_num == 0) return false;
  std::unique_ptr<DXGI_MODE_DESC[]> descs(new DXGI_MODE_DESC[item_num]);
  if (FAILED(
        dxgi_output->GetDisplayModeList(
          desc.Format,
          0,
          &item_num,
          descs.get()))) {
    return false;
  }
  int display_height = descs[0].Height;
  for (int i = 1; i < static_cast<int>(item_num); ++i) {
    if (static_cast<int>(descs[i].Height) > display_height) {
      display_height = static_cast<int>(descs[i].Height);  // Largest value
    }
  }
  display_size->x = static_cast<UINT>(1.3333333 * display_height);  // 1.3=4/3
  display_size->y = display_height;
  return true;
}
bool CheckGraphicDeviceError() {
  switch (graphic_data.device->GetDeviceRemovedReason()) {
    case DXGI_ERROR_DEVICE_HUNG:
      ErrorDialogBox(SYS_ERROR_DEVICE_HUNG);
      return false;
    case DXGI_ERROR_DEVICE_RESET:
      ErrorDialogBox(SYS_ERROR_DEVICE_RESET);
      return false;
    case DXGI_ERROR_DEVICE_REMOVED:
      ErrorDialogBox(SYS_ERROR_DEVICE_REMOVED);
      return false;
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
      ErrorDialogBox(SYS_ERROR_DRIVER_INTERNAL_ERROR);
      return false;
    case DXGI_ERROR_INVALID_CALL:
      ErrorDialogBox(SYS_ERROR_INVALID_CALL);
      return false;
    case S_OK:
    default:
      break;
  }
  return true;
}
bool CheckDisplayModeChange() {
  BOOL is_fullscreen = 0;  // false
  static BOOL was_fullscreen = 0;  // false
  graphic_data.dxgi_swap_chain->GetFullscreenState(&is_fullscreen, nullptr);
  if (is_fullscreen != was_fullscreen) {
    DXGI_MODE_DESC desc;
    desc.RefreshRate.Numerator = SYS_REFRESH_RATE_NUMERATOR;
    desc.RefreshRate.Denominator = SYS_REFRESH_RATE_DENOMINATOR;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    if (is_fullscreen) {
      // Fullcreen mode.
      Vector2<UINT> display_size;
      if (!GetFullscreenSize(&display_size)) return false;
      desc.Width = display_size.x;
      desc.Height = display_size.y;
      desc.Scaling = DXGI_MODE_SCALING_CENTERED;
    } else {
      // Window mode.
      desc.Width = system_data.window_size.x;
      desc.Height = system_data.window_size.y;
      desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    }
    if (FAILED(graphic_data.dxgi_swap_chain->ResizeTarget(&desc))) {
      return false;
    }
    was_fullscreen = is_fullscreen;
  }
  return true;
}
bool InitGraphic() {
  // Graphic pipeline
  if (!CreateD3DDevice()) return false;
  if (!CreateDXGIDevice()) return false;
  if (!CreateSwapChain()) return false;
  if (!CreateBackBuffer()) return false;
  if (!CreateRenderTargetView()) return false;
  if (!SetViewPort()) return false;
  // Buffers
  if (!CreateVSConstBuffer()) return false;
  if (!CreatePSConstBuffer()) return false;
  if (!CreateBlendState()) return false;
  if (!CreateSamplerState()) return false;
  // Shaders
  if (!CreateVertexInputLayout(
        (const BYTE**) g_vshader1,
        sizeof(g_vshader1))) {  // NOLINT, C++ style cast to error
    return false;
  }
  if (!CreateVertexShader(
        (const BYTE**) g_vshader1,
        sizeof(g_vshader1),
        &graphic_data.vertex_shader1)) {  // NOLINT, C++ style cast to error
    return false;
  }
  if (!CreatePixelShader(
        (const BYTE**) g_pshader1,
        sizeof(g_pshader1),
        &graphic_data.pixel_shader1)) {  // NOLINT, C++ style cast to error
    return false;
  }
  // Graphic Pipeline
  if (!CreateGraphicPipeline()) return false;
  // Etc.
  if (!EnableWindowModeChange()) return false;
  if (!SetMaximumFrameLatency()) return false;
  return true;
}
void FinalizeGraphic() {
  if (graphic_data.dxgi_swap_chain != nullptr) {
    graphic_data.dxgi_swap_chain->SetFullscreenState(false, nullptr);
  }
  SYS_SAFE_RELEASE(graphic_data.pixel_shader1);
  SYS_SAFE_RELEASE(graphic_data.vertex_shader1);
  //
  SYS_SAFE_RELEASE(graphic_data.input_layout);
  //
  SYS_SAFE_RELEASE(graphic_data.sampler_state);
  SYS_SAFE_RELEASE(graphic_data.blend_state);
  SYS_SAFE_RELEASE(graphic_data.ps_cbuffer);
  SYS_SAFE_RELEASE(graphic_data.vs_cbuffer);
  //
  SYS_SAFE_RELEASE(graphic_data.render_target_view);
  SYS_SAFE_RELEASE(graphic_data.back_buffer);
  SYS_SAFE_RELEASE(graphic_data.dxgi_swap_chain);
  SYS_SAFE_RELEASE(graphic_data.dxgi_factory);
  SYS_SAFE_RELEASE(graphic_data.dxgi_adaptor);
  SYS_SAFE_RELEASE(graphic_data.dxgi_device);
  SYS_SAFE_RELEASE(graphic_data.device);
}
bool PresentGraphic() {
  HRESULT result = S_OK;
  if (!graphic_data.on_power_save) {
    result = graphic_data.dxgi_swap_chain->Present(1, 0);  // Vsync
  } else {
    result = graphic_data.dxgi_swap_chain->Present(false, DXGI_PRESENT_TEST);
  }
  switch (result) {
    case S_OK:
      graphic_data.on_power_save = false;
      break;
    case DXGI_STATUS_OCCLUDED:
      graphic_data.on_power_save = true;
      break;
    case DXGI_ERROR_DEVICE_REMOVED:
      FinalizeGraphic();
      ErrorDialogBox(SYS_ERROR_DXGI_DEVICE_REMOVED);
      return false;
    default:
      break;
  }
  return true;
}
bool UpdateGraphic() {
  if (!CheckGraphicDeviceError()) return false;
  if (!CheckDisplayModeChange()) return false;
  if (!PresentGraphic()) return false;
  return true;
}
bool CreateTextureData(const TextureDesc& desc, TextureData* texture) {
  assert(texture);
  const ResourceDesc resource_desc = desc.resource_desc;
  D3DX11_IMAGE_INFO image_info;
  D3DX11_IMAGE_LOAD_INFO load_info;
  if (resource_desc.use_mem) {
    return false;  // TODO(Mamoru): Implement memory source one.
  } else {
    // Texture properties aquired.
    if (FAILED(
          D3DX11GetImageInfoFromFile(
            resource_desc.file_name.c_str(),
            nullptr,
            &image_info,
            nullptr))) {
      return false;
    }
    texture->w = image_info.Width;
    texture->h = image_info.Height;
    texture->blend_factor[0] = 1.0f;
    texture->blend_factor[1] = 1.0f;
    texture->blend_factor[2] = 1.0f;
    texture->blend_factor[3] = 1.0f;
    // Load resource
    load_info.Width = texture->w;
    load_info.Height = texture->h;
    load_info.Depth = 0;
    load_info.FirstMipLevel = 0;
    load_info.MipLevels = D3DX11_DEFAULT,
    load_info.Usage = D3D11_USAGE_IMMUTABLE;
    load_info.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    load_info.CpuAccessFlags = 0;
    load_info.MiscFlags = 0;
    load_info.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    load_info.Filter = D3DX11_FILTER_NONE;
    load_info.MipFilter = D3DX11_FILTER_NONE;
    load_info.pSrcInfo = nullptr;
    // ShaderResourceView is created.
    if (FAILED(D3DX11CreateShaderResourceViewFromFile(
            graphic_data.device,
            resource_desc.file_name.c_str(),
            &load_info,
            nullptr,
            texture->shader_resource_view,
            nullptr))) {
      return false;
    }
  }
  return true;
}
bool ReleaseTextureData(TextureData* texture) {
  assert(texture);
  SYS_SAFE_RELEASE(texture);
  return true;
}
bool GetTextureSize(TextureData* texture, Vector2d* size) {
  assert(texture);
  assert(size);
  size->x = static_cast<double>(texture->w);
  size->y = static_cast<double>(texture->h);
  return true;
}
bool CreateImageData(const ImageDesc& desc, TextureData* texture,
                     ImageData* image) {
  assert(texture);
  assert(image);
  // Data is set to image data.
  image->texture_id = desc.texture_id;
  int w = desc.w;
  int h = desc.h;
  if (w == 0) w = texture->w;  // Full texture width is used.
  if (h == 0) h = texture->h;  // Full texture height is used.
  image->w = static_cast<int>(w * desc.s);
  image->h = static_cast<int>(h * desc.s);
  // Image polygon rect size in window set.
  float e_x = static_cast<float>(1.0f / graphic_data.resolution.x * 2.0f);
  float e_y = static_cast<float>(1.0f / graphic_data.resolution.y * 2.0f);
  float a_x = 0.0f;
  float a_y = 0.0f;
  float b_x = static_cast<float>(desc.s * w * e_x);
  float b_y = -static_cast<float>(desc.s * h * e_y);
  float position[SYS_VERTEX_INPUT_NUM][3] = {
    { a_x, a_y, 0.5f },
    { b_x, a_y, 0.5f },
    { a_x, b_y, 0.5f },
    { b_x, b_y, 0.5f },
  };
  // Cordinates from pixel to U-V.
  e_x = static_cast<float>(1.0f / texture->w);
  e_y = static_cast<float>(1.0f / texture->h);
  a_x = static_cast<float>(desc.x * e_x);
  a_y = static_cast<float>(desc.y * e_y);
  b_x = static_cast<float>(a_x + w * e_x);
  b_y = static_cast<float>(a_y + h * e_y);
  float def_coord[SYS_VERTEX_INPUT_NUM][2] = {
    { a_x, a_y },
    { b_x, a_y },
    { a_x, b_y },
    { b_x, b_y },
  };
  // Coordinatess wor texture polygon.
  float coord[SYS_VERTEX_INPUT_NUM][2] = {0};
  switch (desc.image_mode) {
    case SYS_IMAGEMODE_DEFAULT:
      coord[0][0] = def_coord[0][0]; coord[0][1] = def_coord[0][1];
      coord[1][0] = def_coord[1][0]; coord[1][1] = def_coord[1][1];
      coord[2][0] = def_coord[2][0]; coord[2][1] = def_coord[2][1];
      coord[3][0] = def_coord[3][0]; coord[3][1] = def_coord[3][1];
      break;
    case SYS_IMAGEMODE_HINVERT:
      coord[0][0] = def_coord[1][0]; coord[0][1] = def_coord[1][1];
      coord[1][0] = def_coord[0][0]; coord[1][1] = def_coord[0][1];
      coord[2][0] = def_coord[3][0]; coord[2][1] = def_coord[3][1];
      coord[3][0] = def_coord[2][0]; coord[3][1] = def_coord[2][1];
      break;
    case SYS_IMAGEMODE_VINVERT:
      coord[0][0] = def_coord[2][0]; coord[0][1] = def_coord[2][1];
      coord[1][0] = def_coord[3][0]; coord[1][1] = def_coord[3][1];
      coord[2][0] = def_coord[0][0]; coord[2][1] = def_coord[0][1];
      coord[3][0] = def_coord[1][0]; coord[3][1] = def_coord[1][1];
      break;
    case SYS_IMAGEMODE_ROT90:
      coord[0][0] = def_coord[2][0]; coord[0][1] = def_coord[2][1];
      coord[1][0] = def_coord[0][0]; coord[1][1] = def_coord[0][1];
      coord[2][0] = def_coord[3][0]; coord[2][1] = def_coord[3][1];
      coord[3][0] = def_coord[1][0]; coord[3][1] = def_coord[1][1];
      break;
    case SYS_IMAGEMODE_ROT180:
      coord[0][0] = def_coord[3][0]; coord[0][1] = def_coord[3][1];
      coord[1][0] = def_coord[2][0]; coord[1][1] = def_coord[2][1];
      coord[2][0] = def_coord[1][0]; coord[2][1] = def_coord[1][1];
      coord[3][0] = def_coord[0][0]; coord[3][1] = def_coord[0][1];
      break;
    case SYS_IMAGEMODE_ROT270:
      coord[0][0] = def_coord[1][0]; coord[0][1] = def_coord[1][1];
      coord[1][0] = def_coord[3][0]; coord[1][1] = def_coord[3][1];
      coord[2][0] = def_coord[0][0]; coord[2][1] = def_coord[0][1];
      coord[3][0] = def_coord[2][0]; coord[3][1] = def_coord[2][1];
      break;
    default:
      break;
  }
  // Relations set.
  VertexInputData vertices_data[SYS_VERTEX_INPUT_NUM];
  for (int i = 0; i < SYS_VERTEX_INPUT_NUM; ++i) {
    // SV_POSITION
    vertices_data[i].position[0] = position[i][0];
    vertices_data[i].position[1] = position[i][1];
    vertices_data[i].position[2] = position[i][2];
    // TEXCOORD0
    vertices_data[i].texcoord[0] = coord[i][0];
    vertices_data[i].texcoord[1] = coord[i][1];
  }
  // Vertex buffer is created.
  D3D11_BUFFER_DESC buf_desc;
  buf_desc.ByteWidth = sizeof(VertexInputData) * SYS_VERTEX_INPUT_NUM;
  buf_desc.Usage = D3D11_USAGE_DEFAULT;
  buf_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  buf_desc.CPUAccessFlags = 0;
  buf_desc.MiscFlags = 0;
  buf_desc.StructureByteStride = sizeof(VertexInputData);
  D3D11_SUBRESOURCE_DATA data;
  data.pSysMem = vertices_data;
  data.SysMemPitch = 0;
  data.SysMemSlicePitch = 0;
  if (FAILED(
        graphic_data.device->CreateBuffer(
          &buf_desc,
          &data,
          &image->buffer))) {
    return false;
  }
  return true;
}
bool ReleaseImageData(ImageData* image) {
  assert(image);
  SYS_SAFE_RELEASE(image);
  return true;
}
bool GetImageSize(ImageData* image, Vector2d* size) {
  assert(image);
  assert(size);
  size->x = static_cast<double>(image->w);
  size->y = static_cast<double>(image->h);
  return true;
}
bool DrawImageData(TextureData* texture, ImageData* image,
                   const Vector2d& position, const Color4b& color) {
  assert(image);
  assert(texture);
  if (graphic_data.on_power_save) return false;  // Power save state.
  // Graphic pipeline is over written
  UINT strids = sizeof(VertexInputData);
  UINT offsets = 0;
  graphic_data.device_context->IASetVertexBuffers(
      0,
      1,
      &image->buffer,
      &strids,
      &offsets);
  graphic_data.device_context->PSSetShaderResources(
      0,
      1,
      texture->shader_resource_view);
  graphic_data.device_context->OMSetBlendState(
      graphic_data.blend_state,
      texture->blend_factor,
      0xffffffff);
  // Vertex shader constant buffer overwritten
  XMMATRIX world_matrix = XMMatrixIdentity();
  float display_x =
      static_cast<float>(position.x / graphic_data.resolution.x * 2.0f - 1.0f);
  float display_y =
      static_cast<float>(-position.y / graphic_data.resolution.y * 2.0f + 1.0f);
  XMMATRIX matrix = XMMatrixTranslation(display_x, display_y, 0.5f);
  VSConstBufferData vs_buffer;
  vs_buffer.world_matrix =
    XMMatrixTranspose(XMMatrixMultiply(world_matrix, matrix));
  graphic_data.device_context->UpdateSubresource(
      graphic_data.vs_cbuffer,
      0,
      nullptr,
      &vs_buffer,
      0,
      0);
  graphic_data.device_context->VSSetConstantBuffers(
      0,
      1,
      &graphic_data.vs_cbuffer);
  // Pixel shader constant buffer overwritten
  PSConstBufferData ps_buffer;
  ps_buffer.source_color.x = color.x * 0.0039215686f;  // R, div by 255
  ps_buffer.source_color.y = color.y * 0.0039215686f;  // G, div by 255
  ps_buffer.source_color.z = color.z * 0.0039215686f;  // B, div by 255
  ps_buffer.source_color.w = color.w * 0.0039215686f;  // A, div by 255
  graphic_data.device_context->UpdateSubresource(
      graphic_data.ps_cbuffer,
      0,
      nullptr,
      &ps_buffer,
      0,
      0);
  graphic_data.device_context->PSSetConstantBuffers(
      0,
      1,
      &graphic_data.ps_cbuffer);
  graphic_data.device_context->Draw(SYS_VERTEX_INPUT_NUM, 0);
  return true;
}
bool CreateFontData(const FontDesc& desc, FontData* font) {
  assert(font);
  TextureDesc texture_desc;
  texture_desc.resource_desc = desc.resource_desc;
  if (!CreateTextureData(texture_desc, &font->font_texture)) return false;
  // Image created for font.
  int w = font->font_texture.w / SYS_FONT_COLUMN_NUM;
  int h = font->font_texture.h / SYS_FONT_ROW_NUM;
  ImageDesc image_desc;
  image_desc.image_mode = desc.image_mode;
  image_desc.s = desc.s;
  for (int i = 0; i < SYS_FONT_ROW_NUM; ++i) {
    for (int j = 0; j < SYS_FONT_COLUMN_NUM; ++j) {
      image_desc.x = w * j;
      image_desc.y = h * i;
      image_desc.w = w;
      image_desc.h = h;
      if (!CreateImageData(
            image_desc,
            &font->font_texture,
            &font->font_image[i * SYS_FONT_COLUMN_NUM + j])) {
        return false;
      }
    }
  }
  return true;
}
bool ReleaseFontData(FontData* font) {
  assert(font);
  font->Release();
  return true;
}
bool GetFontSize(FontData* font, Vector2d* size) {
  assert(font);
  GetImageSize(&font->font_image[0], size);
  return true;
}
bool GetTextSize(FontData* font, Vector2d* size, const wchar_t* text) {
  assert(font);
  assert(size);
  assert(text);
  //
  Vector2d image_size;
  GetImageSize(&font->font_image[0], &image_size);
  size->x = image_size.x * wcslen(text);
  size->y = image_size.y;
  return true;
}
bool DrawTextData(FontData* font, const Vector2d& position,
                  const Color4b& color, SYS_FONTMODE font_mode,
                  const wchar_t* text) {
  assert(font);
  assert(text);
  if (graphic_data.on_power_save) return false;
  //
  Vector2d text_size;
  GetTextSize(font, &text_size, text);
  Vector2d draw_position = position;
  switch (font_mode) {
    case SYS_FONTMODE_TOP_LEFT:
      draw_position = position;
      break;
    case SYS_FONTMODE_TOP_CENTER:
      draw_position.sub(Vector2d(text_size.x * 0.5, 0.0));
      break;
    case SYS_FONTMODE_TOP_RIGHT:
      draw_position.sub(Vector2d(text_size.x, 0.0));
      break;
    case SYS_FONTMODE_MIDDLE_LEFT:
      draw_position.sub(Vector2d(0.0, text_size.y * 0.5));
      break;
    case SYS_FONTMODE_MIDDLE_CENTER:
      draw_position.sub(Vector2d(text_size.x * 0.5, text_size.y * 0.5));
      break;
    case SYS_FONTMODE_MIDDLE_RIGHT:
      draw_position.sub(Vector2d(text_size.x, text_size.y * 0.5));
      break;
    case SYS_FONTMODE_BOTTOM_LEFT:
      draw_position.sub(Vector2d(0.0, text_size.y));
      break;
    case SYS_FONTMODE_BOTTOM_CENTER:
      draw_position.sub(Vector2d(text_size.x * 0.5, text_size.y));
      break;
    case SYS_FONTMODE_BOTTOM_RIGHT:
      draw_position.sub(Vector2d(text_size.x, text_size.y));
      break;
  }
  const wchar_t font_table[SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM] =
    L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\] ";
  Vector2d font_size;
  GetFontSize(font, &font_size);
  int text_length = wcslen(text);
  ImageData* font_image = nullptr;
  for (int i = 0; i < text_length; ++i) {
    font_image = &font->font_image[0];  // space ' '
    for (int j = 0; j < SYS_FONT_ROW_NUM * SYS_FONT_COLUMN_NUM; ++j) {
      if (text[i] == font_table[j]) {
        font_image = &font->font_image[j];
        break;
      }
    }
    if (!DrawImageData(
          &font->font_texture,
          font_image,
          Vector2d(draw_position.x + font_size.x * i, draw_position.y),
          color)) {
      return false;
    }
  }
  return true;
}
////////////////////////////////////////
// Sound
////////////////////////////////////////
#define SYS_WAVE_BUFFER_ELEM_NUM  (256)
#define SYS_STREAMING_READ_BYTES  (32768)  // 2 ^ 15
struct WaveData {
  IDirectSoundBuffer* buffer;
  WaveData() : buffer(nullptr) { }
  void Release() {
    SYS_SAFE_RELEASE(buffer);
  }
  bool IsNull() {
    return (buffer == nullptr);
  }
};
struct StreamingData {
  ResourceDesc resource_desc;
  HANDLE hthread;
  bool stop_request;
  bool in_pause;
  bool in_use;
  bool in_loop;
  WaveData wave_data;
  StreamingData() : resource_desc(), hthread(nullptr), wave_data(),
      stop_request(false), in_pause(false), in_use(false), in_loop(false) { }
  void Reset() {
    hthread = nullptr;
    stop_request = false;
    in_pause = false;
    in_use = false;
    in_loop = false;
    wave_data.Release();
  }
};
struct SoundData {
  IDirectSound8* direct_sound8;
  HANDLE hmutex;
  WaveData wave_buffer[SYS_WAVE_BUFFER_ELEM_NUM];
  StreamingData streaming_data;
  SoundData() : direct_sound8(nullptr), hmutex(nullptr), wave_buffer(),
      streaming_data() { }
};
SoundData sound_data;
HMMIO GetFileHandle(const wchar_t* file_name) {
  return mmioOpen((LPWSTR) file_name, nullptr, MMIO_READ);
}
HMMIO GetFileHandle(const HPSTR mem_ptr, int mem_size) {
  MMIOINFO mmio_info;
  memset(&mmio_info, 0, sizeof(mmio_info));
  mmio_info.pchBuffer = mem_ptr;
  mmio_info.fccIOProc = FOURCC_MEM;
  mmio_info.cchBuffer = mem_size;
  mmio_info.adwInfo[0] = 1024;
  return mmioOpen(nullptr, &mmio_info, MMIO_READ);
}
bool DescendToRiffChunk(HMMIO hmmio, MMCKINFO* riff_chunk_out) {
  if (!riff_chunk_out) return false;
  memset(riff_chunk_out, 0, sizeof(MMCKINFO));
  riff_chunk_out->fccType = mmioFOURCC('W', 'A', 'V', 'E');
  if (mmioDescend(hmmio, riff_chunk_out, nullptr, MMIO_FINDRIFF) !=
      MMSYSERR_NOERROR) {
    return false;
  }
  return true;
}
bool DescendToFormatChunk(HMMIO hmmio, const MMCKINFO& riff_chunk,
                          MMCKINFO* format_chunk_out) {
  memset(format_chunk_out, 0, sizeof(MMCKINFO));
  format_chunk_out->ckid = mmioFOURCC('f', 'm', 't', ' ');
  if (mmioDescend(hmmio, format_chunk_out, &riff_chunk, MMIO_FINDCHUNK) !=
      MMSYSERR_NOERROR) {
    return false;
  }
  return true;
}
bool ReadWavFormatEx(HMMIO hmmio, const MMCKINFO& format_chunk,
                     WAVEFORMATEX* wave_fmt_ex_out) {
  memset(wave_fmt_ex_out, 0, sizeof(WAVEFORMATEX));
  if (mmioRead(hmmio, (HPSTR) wave_fmt_ex_out, format_chunk.cksize) !=
      static_cast<LONG>(format_chunk.cksize)) {
    return false;
  }
  return true;
}
bool DescendToDataChunk(HMMIO hmmio, const MMCKINFO& riff_chunk,
                        MMCKINFO* data_chunk_out) {
  memset(data_chunk_out, 0, sizeof(MMCKINFO));
  data_chunk_out->ckid = mmioFOURCC('d', 'a', 't', 'a');
  if (mmioDescend(hmmio, data_chunk_out, &riff_chunk, MMIO_FINDCHUNK) !=
      MMSYSERR_NOERROR) {
    return false;
  }
  return true;
}
bool CreateWaveDataBuffer(const char* wave_mem, int wave_data_size,
                          WaveData* wave, WAVEFORMATEX* wave_fmt_ex) {
  assert(wave);
  //
  DSBUFFERDESC ds_buf_desc;
  memset(&ds_buf_desc, 0, sizeof(ds_buf_desc));
  ds_buf_desc.dwSize = sizeof(ds_buf_desc);
  ds_buf_desc.dwFlags = DSBCAPS_GLOBALFOCUS;
  ds_buf_desc.dwBufferBytes = wave_data_size;
  ds_buf_desc.lpwfxFormat = static_cast<LPWAVEFORMATEX>(wave_fmt_ex);
  ds_buf_desc.guid3DAlgorithm = GUID_NULL;
  IDirectSoundBuffer* temp_buffer;
  if (FAILED(
        sound_data.direct_sound8->CreateSoundBuffer(
          &ds_buf_desc,
          &temp_buffer,
          nullptr))) {
    return false;
  }
  temp_buffer->QueryInterface(
      IID_IDirectSoundBuffer8,
      reinterpret_cast<void**>(&wave->buffer));
  SYS_SAFE_RELEASE(temp_buffer);
  //
  if (wave->buffer == nullptr) return false;
  LPVOID write_ptr = nullptr;
  DWORD length = 0;
  if (FAILED(
        wave->buffer->Lock(
          0,
          0,
          &write_ptr,
          &length,
          nullptr,
          nullptr,
          DSBLOCK_ENTIREBUFFER))) {
    return false;
  }
  memcpy(write_ptr, wave_mem, length);
  wave->buffer->Unlock(write_ptr, length, nullptr, 0);
  return true;
}
bool CreateWaveData(const WaveDesc& desc, WaveData* wave) {
  assert(wave);
  const ResourceDesc resource_desc = desc.resource_desc;
  HMMIO hmmio = nullptr;
  if (resource_desc.use_mem) {
    // From memory
    hmmio = GetFileHandle(
        reinterpret_cast<const HPSTR>(resource_desc.mem_ptr),
        resource_desc.mem_size);
  } else {
    // From file
    hmmio = GetFileHandle(resource_desc.file_name.c_str());
  }
  if (hmmio == nullptr) return false;
  MMCKINFO riff_chunk = {0};
  if (!DescendToRiffChunk(hmmio, &riff_chunk)) {
    mmioClose(hmmio, 0);
    return false;
  }
  MMCKINFO format_chunk = {0};
  if (!DescendToFormatChunk(hmmio, riff_chunk, &format_chunk)) {
    mmioClose(hmmio, 0);
    return false;
  }
  WAVEFORMATEX wave_fmt_ex = {0};
  if (!ReadWavFormatEx(hmmio, format_chunk, &wave_fmt_ex)) {
    mmioClose(hmmio, 0);
    return false;
  }
  mmioAscend(hmmio, &format_chunk, 0);
  MMCKINFO data_chunk = {0};
  if (!DescendToDataChunk(hmmio, riff_chunk, &data_chunk)) {
    mmioClose(hmmio, 0);
    return false;
  }
  std::unique_ptr<char[]> wave_mem(new char[data_chunk.cksize]);
  if (mmioRead(hmmio, (HPSTR) wave_mem.get(), data_chunk.cksize) !=
      static_cast<LONG>(data_chunk.cksize)) {
    mmioClose(hmmio, 0);
    return false;
  }
  mmioClose(hmmio, 0);
  // Sound buffer is set.
  WaitForSingleObject(sound_data.hmutex, 0);
  if (!CreateWaveDataBuffer(
        wave_mem.get(),
        data_chunk.cksize,
        wave,
        &wave_fmt_ex)) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool StopWaveData(WaveData* wave) {
  assert(wave);
  WaitForSingleObject(sound_data.hmutex, 0);
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  wave->buffer->Stop();
  wave->buffer->SetCurrentPosition(0);
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool ReleaseWaveData(WaveData* wave) {
  assert(wave);
  StopWaveData(wave);
  WaitForSingleObject(sound_data.hmutex, 0);
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  wave->Release();
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool PlayWaveData(WaveData* wave) {
  assert(wave);
  WaitForSingleObject(sound_data.hmutex, 0);
  if (wave->IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  wave->buffer->Play(0, 0, 0);
  ReleaseMutex(sound_data.hmutex);
  return true;
}
unsigned __stdcall StreamingProc(LPVOID lpargs) {
  assert(lpargs);
  ResourceDesc* resource_desc = static_cast<ResourceDesc*>(lpargs);
  HMMIO hmmio = nullptr;
  do {
    if (resource_desc->use_mem) {
      // Form memory
      hmmio = GetFileHandle(
          reinterpret_cast<const HPSTR>(resource_desc->mem_ptr),
          resource_desc->mem_size);
    } else {
      // From file
      hmmio = GetFileHandle(resource_desc->file_name.c_str());
    }
    if (hmmio == nullptr) return false;
    MMCKINFO riff_chunk = {0};
    if (!DescendToRiffChunk(hmmio, &riff_chunk)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    MMCKINFO format_chunk = {0};
    if (!DescendToFormatChunk(hmmio, riff_chunk, &format_chunk)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    WAVEFORMATEX wave_fmt_ex = {0};
    if (!ReadWavFormatEx(hmmio, format_chunk, &wave_fmt_ex)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    mmioAscend(hmmio, &format_chunk, 0);
    MMCKINFO data_chunk = {0};
    if (!DescendToDataChunk(hmmio, riff_chunk, &data_chunk)) {
      mmioClose(hmmio, 0);
      return S_OK;
    }
    DWORD buf_status = 0;
    std::unique_ptr<char[]> wave_mem(new char[SYS_STREAMING_READ_BYTES]);
    int read_size = 0;
    while (true) {
      if (sound_data.streaming_data.stop_request) {
        mmioClose(hmmio, 0);
        return S_OK;  // Thread terminated.
        break;
      }
      if (!sound_data.streaming_data.in_pause &&
          !(buf_status & DSBSTATUS_PLAYING)) {
        read_size = mmioRead(hmmio, (HPSTR) wave_mem.get(),
                             SYS_STREAMING_READ_BYTES);
        if (read_size == 0) {
          break;
        }
        if (!CreateWaveDataBuffer(
              wave_mem.get(),
              read_size,
              &sound_data.streaming_data.wave_data,
              &wave_fmt_ex)) {
          break;
        }
        PlayWaveData(&sound_data.streaming_data.wave_data);
      }
      sound_data.streaming_data.wave_data.buffer->GetStatus(&buf_status);
    }
    mmioClose(hmmio, 0);
  } while (sound_data.streaming_data.in_loop);
  return S_OK;  // Thread terminated.
}
bool InitSound() {
  if (FAILED(
        DirectSoundCreate8(
          nullptr,
          &sound_data.direct_sound8,
          nullptr))) {
    return false;
  }
  // Coperative mode is designated.
  if (FAILED(
        sound_data.direct_sound8->SetCooperativeLevel(
          system_data.hwnd,
          DSSCL_PRIORITY))) {
    return false;
  }
  sound_data.streaming_data.Reset();
  sound_data.hmutex = CreateMutex(nullptr, TRUE, nullptr);
  return true;
}
void FinalizeSound() {
  StopStreaming();
  CloseHandle(sound_data.hmutex);
  SYS_SAFE_RELEASE(sound_data.direct_sound8);
}
bool UpdateSound() {
  return true;  // No Impl.
}
////////////////////////////////////////
// Input
////////////////////////////////////////
#define SYS_JOYPAD_RANGE_MAX          (1000)
#define SYS_JOYPAD_THRESHOLD_DEFAULT  (50)
struct InputStatus {
  virtual ~InputStatus() { }
  virtual bool IsON(size_t key) = 0;
  virtual void Reset() = 0;
};
struct KeyboardStatus : public InputStatus {
  char status[SYS_ELEMNUM_KEYID];
  KeyboardStatus() : status() { }
  KeyboardStatus(const KeyboardStatus& keyboard_status) {
    memcpy(status, keyboard_status.status, sizeof(status));
  }
  bool IsON(size_t key) {
    if ((key < 0) || (key >= SYS_ELEMNUM_KEYID)) {
      ErrorDialogBox(SYS_ERROR_INVALID_KEY, key);
      return false;
    }
    return (status[key] != 0);
  }
  void Reset() {
    memset(status, 0, sizeof(status));
  }
};
struct JoypadStatus : public InputStatus {
  char status[32];
  int analog_stick[2];
  int threshold_x;
  int threshold_y;
  JoypadStatus() :
      status(),
      analog_stick(),
      threshold_x(SYS_JOYPAD_THRESHOLD_DEFAULT),
      threshold_y(SYS_JOYPAD_THRESHOLD_DEFAULT) { }
  JoypadStatus(const JoypadStatus& joypad_status) {
    memcpy(status, joypad_status.status, sizeof(status));
    memcpy(analog_stick, joypad_status.analog_stick, sizeof(analog_stick));
  }
  bool IsON(size_t key) {
    switch (key) {
      case SYS_JOYPAD_KEY_DOWN:
        return (analog_stick[1] > threshold_y);
      case SYS_JOYPAD_KEY_LEFT:
        return (analog_stick[0] < -threshold_x);
      case SYS_JOYPAD_KEY_RIGHT:
        return (analog_stick[0] > threshold_x);
      case SYS_JOYPAD_KEY_UP:
        return (analog_stick[1] < -threshold_y);
      default:
        if ((key < 0) || (key >= 32)) {
          ErrorDialogBox(SYS_ERROR_INVALID_JOYPAD_KEY, key);
          return false;
        }
        return (status[key] != 0);
    }
  }
  void Reset() {
    memset(status, 0, sizeof(status));
    memset(analog_stick, 0, sizeof(analog_stick));
  }
};
struct VirtualStatus : public InputStatus {
  char status[SYS_ELEMNUM_BUTTON_KEY];
  VirtualStatus() : status() { }
  VirtualStatus(const VirtualStatus& virtual_status) {
    memcpy(status, virtual_status.status, sizeof(status));
  }
  bool IsON(size_t key) {
    if ((key < 0) || (key >= SYS_ELEMNUM_BUTTON_KEY)) {
      ErrorDialogBox(SYS_ERROR_INVALID_VIRTUAL_KEY, key);
      return false;
    }
    return (status[key] != 0);
  }
  void Reset() {
    memset(status, 0, sizeof(status));
  }
};
struct InputData {
  IDirectInput8* keyboard8;
  IDirectInput8* joypad8;
  IDirectInputDevice8* keyboard_device;
  IDirectInputDevice8* joypad_device;
  //
  bool keyboard_available;
  bool joypad_available;
  KeyboardStatus keyboard_status;
  JoypadStatus joypad_status;
  VirtualStatus virtual_status;
  VirtualStatus last_virtual_status;
  VirtualStatus virtual_pressed;
  std::map<int, int> key_map;  // <key, button key>
  std::map<int, int> joypad_key_map;  // <joypad key, button key>
  InputData() :
      keyboard8(nullptr),
      joypad8(nullptr),
      keyboard_device(nullptr),
      joypad_device(nullptr),
      keyboard_available(false),
      joypad_available(false) {
    // Fixed virtual key assign for keyboard.
    key_map.insert(std::make_pair(SYS_KEY_DOWN, SYS_VIRTUAL_KEY_DOWN));
    key_map.insert(std::make_pair(SYS_KEY_LEFT, SYS_VIRTUAL_KEY_LEFT));
    key_map.insert(std::make_pair(SYS_KEY_RIGHT, SYS_VIRTUAL_KEY_RIGHT));
    key_map.insert(std::make_pair(SYS_KEY_UP, SYS_VIRTUAL_KEY_UP));
    // Fixed virtual key assign for joypad.
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_DOWN, SYS_VIRTUAL_KEY_DOWN));
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_LEFT, SYS_VIRTUAL_KEY_LEFT));
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_RIGHT, SYS_VIRTUAL_KEY_RIGHT));
    joypad_key_map.insert(
        std::make_pair(SYS_JOYPAD_KEY_UP, SYS_VIRTUAL_KEY_UP));
  }
};
InputData input_data;
bool AquireKeyboard() {
  if (!input_data.keyboard_device) return false;
  if (FAILED(input_data.keyboard_device->Acquire())) return false;
  return true;
}
bool AquireJoypad() {
  if (!input_data.joypad_device) return false;
  if (FAILED(input_data.joypad_device->Acquire())) return false;
  return true;
}
bool InitKeyboard() {
  if (FAILED(
        CoCreateInstance(
          CLSID_DirectInput8,
          nullptr,
          CLSCTX_INPROC_SERVER,
          IID_IDirectInput8,
          reinterpret_cast<void**>(&input_data.keyboard8)))) {
    return false;
  }
  if (!input_data.keyboard8) return false;
  if (FAILED(
        input_data.keyboard8->Initialize(
          system_data.hinstance,
          DIRECTINPUT_VERSION))) {
    return false;
  }
  // Keyboard device interface aquired.
  if (FAILED(
        input_data.keyboard8->CreateDevice(
          GUID_SysKeyboard,
          &input_data.keyboard_device,
          nullptr))) {
    return false;
  }
  if (!input_data.keyboard_device) return false;
  // Data format set.
  if (FAILED(
        input_data.keyboard_device->SetDataFormat(
          &c_dfDIKeyboard))) {
    return false;
  }
  // Cooperative mode set.
  if (FAILED(
        input_data.keyboard_device->SetCooperativeLevel(
          system_data.hwnd,
          DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
    return false;
  }
  return true;
}
BOOL CALLBACK EnumJoysticProc(const DIDEVICEINSTANCE* pdidInstance,
                              void* context) {
  if (FAILED(
        input_data.joypad8->CreateDevice(
          pdidInstance->guidInstance,
          &input_data.joypad_device,
          nullptr))) {
    return DIENUM_CONTINUE;
  }
  UNREFERENCED_PARAMETER(context);
  return DIENUM_STOP;
}
BOOL CALLBACK EnumAxisProc(const DIDEVICEOBJECTINSTANCE* pdidoi,
                           void* context) {
  DIPROPRANGE range;
  memset(&range, 0, sizeof(range));
  range.diph.dwSize = sizeof(range);
  range.diph.dwHeaderSize = sizeof(range.diph);
  range.diph.dwHow = DIPH_BYID;
  range.diph.dwObj = pdidoi->dwType;
  range.lMin = -SYS_JOYPAD_RANGE_MAX;
  range.lMax = SYS_JOYPAD_RANGE_MAX;
  if (FAILED(
        input_data.joypad_device->SetProperty(
          DIPROP_RANGE,
          &(range.diph)))) {
    return DIENUM_STOP;
  }
  UNREFERENCED_PARAMETER(context);
  return DIENUM_CONTINUE;
}
int InitJoypad() {
  if (FAILED(
        CoCreateInstance(
          CLSID_DirectInput8,
          nullptr,
          CLSCTX_INPROC_SERVER,
          IID_IDirectInput8,
          reinterpret_cast<void**>(&input_data.joypad8)))) {
    return false;
  }
  if (input_data.joypad8 == nullptr) return false;
  if (FAILED(
        input_data.joypad8->Initialize(
          system_data.hinstance,
          DIRECTINPUT_VERSION))) {
    return false;
  }
  // Pad device interface aquired.
  if (FAILED(
        input_data.joypad8->EnumDevices(
          DI8DEVCLASS_GAMECTRL,
          EnumJoysticProc,
          nullptr,
          DIEDFL_ATTACHEDONLY))) {
    return false;  // No joypad found
  }
  if (input_data.joypad_device == nullptr) return false;
  // Data format set.
  if (FAILED(input_data.joypad_device->SetDataFormat(&c_dfDIJoystick))) {
    return false;
  }
  // Set cooperative mode
  if (FAILED(
        input_data.joypad_device->SetCooperativeLevel(
          system_data.hwnd,
          DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))) {
      return false;
  }
  // Descrive pad config
  if (FAILED(
        input_data.joypad_device->EnumObjects(
          EnumAxisProc,
          nullptr,
          DIDFT_AXIS))) {
    return false;
  }
  // Axis mode is set.
  DIPROPDWORD diprop;
  diprop.diph.dwSize = sizeof(diprop);
  diprop.diph.dwHeaderSize = sizeof(diprop.diph);
  diprop.diph.dwObj = 0;
  diprop.diph.dwHow = DIPH_DEVICE;
  diprop.dwData = DIPROPAXISMODE_ABS;
  if (FAILED(
        input_data.joypad_device->SetProperty(
          DIPROP_AXISMODE,
          &diprop.diph))) {
    return false;
  }
  return true;
}
void FinalizeKeyboard() {
  if (input_data.keyboard_device != nullptr) {
    input_data.keyboard_device->Unacquire();
  }
  SYS_SAFE_RELEASE(input_data.keyboard_device);
  SYS_SAFE_RELEASE(input_data.keyboard8);
}
void ReleaseJoypad() {
  if (input_data.joypad_device != nullptr) {
    input_data.joypad_device->Unacquire();
  }
  SYS_SAFE_RELEASE(input_data.joypad_device);
  SYS_SAFE_RELEASE(input_data.joypad8);
}
bool UpdateKeyboardStatus() {
  input_data.keyboard_status.Reset();
  if (FAILED(
        input_data.keyboard_device->GetDeviceState(
          SYS_ELEMNUM_KEYID,
          input_data.keyboard_status.status))) {
    return false;
  }
  return true;
}
bool UpdateJoypadStatus() {
  input_data.joypad_status.Reset();
  if (FAILED(input_data.joypad_device->Poll())) return false;
  DIJOYSTATE jstate;
  memset(&jstate, 0, sizeof(jstate));
  if (FAILED(
        input_data.joypad_device->GetDeviceState(
          sizeof(jstate),
          &jstate))) {
    return false;
  }
  for (int i = 0; i < 32; ++i) {
    input_data.joypad_status.status[i] =
      static_cast<char>(jstate.rgbButtons[i]);
  }
  input_data.joypad_status.analog_stick[0] = static_cast<int>(jstate.lX);
  input_data.joypad_status.analog_stick[1] = static_cast<int>(jstate.lY);
  return true;
}
bool UpdateVirtualInputStatus() {
  input_data.virtual_status.Reset();
  // User assigned
  for (auto it : input_data.key_map) {
    if (input_data.keyboard_status.IsON(it.first)) {
      input_data.virtual_status.status[it.second] = 1;
    }
  }
  for (auto it : input_data.joypad_key_map) {
    if (input_data.joypad_status.IsON(it.first)) {
      input_data.virtual_status.status[it.second] = 1;
    }
  }
  return true;
}
bool UpdateVirtualInputPressed() {
  input_data.virtual_pressed.Reset();
  for (int i = 0; i < SYS_ELEMNUM_BUTTON_KEY; ++i) {
    if ((input_data.virtual_status.status[i] == 1) &&
        (input_data.last_virtual_status.status[i] == 0)) {
      input_data.virtual_pressed.status[i] = 1;
    }
  }
  input_data.last_virtual_status = input_data.virtual_status;
  return true;
}
bool InitInput() {
  if (InitKeyboard()) input_data.keyboard_available = true;
  if (InitJoypad()) input_data.joypad_available = true;
  if (!input_data.keyboard_available && !input_data.joypad_available) {
    ErrorDialogBox(SYS_ERROR_NO_INPUT_DEVICE);
    return false;
  }
  return true;
}
void FinalizeInput() {
  FinalizeKeyboard();
  ReleaseJoypad();
}
bool UpdateInput() {
  if (input_data.keyboard_available) {
    if (AquireKeyboard()) UpdateKeyboardStatus();
  }
  if (input_data.joypad_available) {
    if (AquireJoypad()) UpdateJoypadStatus();
  }
  UpdateVirtualInputStatus();
  UpdateVirtualInputPressed();
  return true;
}
}  // namespace
////////////////////////////////////////
// System
////////////////////////////////////////
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
////////////////////////////////////////
// Graphics
////////////////////////////////////////
void SetResolution(int x, int y) {
  graphic_data.resolution.x = x;
  graphic_data.resolution.y = y;
}
void StartWithFullscreen(bool fullscreen) {
  graphic_data.on_fullscreen_start = fullscreen;
}
bool FillScreen(const Color4b& color) {
  if (graphic_data.on_power_save) return true;
  float f4[4] = {
    static_cast<float>(color.x / 255.0f),
    static_cast<float>(color.y / 255.0f),
    static_cast<float>(color.z / 255.0f),
    static_cast<float>(color.w / 255.0f),
  };
  graphic_data.device_context->ClearRenderTargetView(
      graphic_data.render_target_view,
      f4);
  return true;
}
bool CreateTexture(const TextureDesc& desc, int texture_id) {
  // 1. Range check
  if ((texture_id < 0) || (texture_id >= SYS_TEXTURE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_TEXTURE_ID, texture_id);
    return false;
  }
  // 2. Duplicate check
  if (!graphic_data.texture_buffer[texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_DUPLICATE_TEXTURE_ID, texture_id);
    return false;
  }
  return CreateTextureData(desc, &graphic_data.texture_buffer[texture_id]);
}
bool ReleaseTexture(int texture_id) {
  // 1. Range check
  if ((texture_id < 0) || (texture_id >= SYS_TEXTURE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_TEXTURE_ID, texture_id);
    return false;
  }
  // 2. Null check
  if (graphic_data.texture_buffer[texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, texture_id);
    return false;
  }
  return ReleaseTextureData(&graphic_data.texture_buffer[texture_id]);
}
bool GetTextureSize(int texture_id, Vector2d* size) {
  // 1. Range check
  if ((texture_id < 0) || (texture_id >= SYS_TEXTURE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_TEXTURE_ID, texture_id);
    return false;
  }
  // 2. Null check
  if (graphic_data.texture_buffer[texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, texture_id);
    return false;
  }
  return GetTextureSize(&graphic_data.texture_buffer[texture_id], size);
}
bool CreateImage(const ImageDesc& desc, int image_id) {
  // 1. Range check (texture)
  if ((desc.texture_id < 0) ||
      (desc.texture_id >= SYS_TEXTURE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_TEXTURE_ID, desc.texture_id);
    return false;
  }
  // 2. Null check (texture)
  if (graphic_data.texture_buffer[desc.texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, desc.texture_id);
    return false;
  }
  // 3. Range check (image)
  if ((image_id < 0) || (image_id >= SYS_IMAGE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 4. Duplicate check (image)
  if (!graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_DUPLICATE_IMAGE_ID, image_id);
    return false;
  }
  return CreateImageData(desc, &graphic_data.texture_buffer[desc.texture_id],
                         &graphic_data.image_buffer[image_id]);
}
bool ReleaseImage(int image_id) {
  // 1. Range check (Image)
  if ((image_id < 0) || (image_id >= SYS_IMAGE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 2. Null check (Image)
  if (graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_IMAGE_ID, image_id);
    return false;
  }
  return ReleaseImageData(&graphic_data.image_buffer[image_id]);
}
bool GetImageSize(int image_id, Vector2d* size) {
  // 1. Range check (Image)
  if ((image_id < 0) || (image_id >= SYS_IMAGE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 2. Null check (Image)
  if (graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_IMAGE_ID, image_id);
    return false;
  }
  return GetImageSize(&graphic_data.image_buffer[image_id], size);
}
bool DrawImage(int image_id, const Vector2d& position, const Color4b& color) {
  // 1. Range check (Image)
  if ((image_id < 0) || (image_id >= SYS_IMAGE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 2. Null check (Image)
  if (graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_IMAGE_ID, image_id);
    return false;
  }
  // 3. Null check (Texture)
  const int texture_id = graphic_data.image_buffer[image_id].texture_id;
  TextureData* texture = &graphic_data.texture_buffer[texture_id];
  if (texture->IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, texture_id);
    return false;
  }
  return DrawImageData(texture, &graphic_data.image_buffer[image_id], position,
                       color);
}
bool CreateFont(const FontDesc& desc, int font_id) {
  // 1. Range check (Font)
  if ((font_id < 0) || (font_id >= SYS_FONT_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Duplicate check (Font)
  if (!graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_DUPLICATE_FONT_ID, font_id);
    return false;
  }
  return CreateFontData(desc, &graphic_data.font_buffer[font_id]);
}
bool ReleaseFont(int font_id) {
  // 1. Range check (font)
  if ((font_id < 0) || (font_id >= SYS_FONT_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check (Font)
  if (graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_FONT_ID, font_id);
    return false;
  }
  return ReleaseFontData(&graphic_data.font_buffer[font_id]);
}
bool GetFontSize(int font_id, Vector2d* size) {
  // 1. Range check (Font)
  if ((font_id < 0) || (font_id >= SYS_FONT_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check (Font)
  if (graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_FONT_ID, font_id);
    return false;
  }
  return GetFontSize(&graphic_data.font_buffer[font_id], size);
}
bool GetTextSize(int font_id, Vector2d* size, const wchar_t* format, ...) {
  // 1. Range check (Font)
  if ((font_id < 0) || (font_id >= SYS_FONT_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check (Font)
  if (graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_FONT_ID, font_id);
    return false;
  }
  wchar_t buffer[256] = {0};
  va_list args;
  va_start(args, format);
  vswprintf_s(buffer, 256, format, args);
  return GetTextSize(&graphic_data.font_buffer[font_id], size, buffer);
}
bool DrawText(int font_id, const Vector2d& position, const Color4b& color,
              SYS_FONTMODE font_mode, const wchar_t* format, ...) {
  // 1. Range check (Font)
  if ((font_id < 0) || (font_id >= SYS_FONT_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check (Font)
  if (graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_FONT_ID, font_id);
    return false;
  }
  wchar_t buffer[256] = {0};
  va_list args;
  va_start(args, format);
  vswprintf_s(buffer, 256, format, args);
  return DrawTextData(&graphic_data.font_buffer[font_id], position, color,
                      font_mode, buffer);
}
////////////////////////////////////////
// Sound
////////////////////////////////////////
bool CreateWave(const WaveDesc& desc, int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Duplicate check (Wave)
  if (!sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_DUPLICATE_WAVE_ID, wave_id);
    return false;
  }
  CreateWaveData(desc, &sound_data.wave_buffer[wave_id]);
  return true;
}
bool StopWave(int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Null check (Wave)
  if (sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_WAVE_ID, wave_id);
    return false;
  }
  return StopWaveData(&sound_data.wave_buffer[wave_id]);
}
bool ReleaseWave(int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Null check (Wave)
  if (sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_WAVE_ID, wave_id);
    return false;
  }
  return ReleaseWaveData(&sound_data.wave_buffer[wave_id]);
}
bool PlayWave(int wave_id) {
  // 1. Range check (Wave)
  if ((wave_id < 0) || (wave_id >= SYS_WAVE_BUFFER_ELEM_NUM)) {
    ErrorDialogBox(SYS_ERROR_INVALID_WAVE_ID, wave_id);
    return false;
  }
  // 2. Null check (Wave)
  if (sound_data.wave_buffer[wave_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_WAVE_ID, wave_id);
    return false;
  }
  return PlayWaveData(&sound_data.wave_buffer[wave_id]);
}
bool PlayStreaming(const StreamingDesc& desc) {
  if (sound_data.streaming_data.in_use) return false;
  sound_data.streaming_data.Reset();
  //
  sound_data.streaming_data.resource_desc = desc.resource_desc;
  sound_data.streaming_data.in_loop = desc.use_loop;
  unsigned thread_id = 0;
  sound_data.streaming_data.hthread =
    (HANDLE) _beginthreadex(
      nullptr,
      0,
      StreamingProc,
      &(sound_data.streaming_data.resource_desc),
      0,
      &thread_id);  // NOLINT
  if (sound_data.streaming_data.hthread == nullptr) return false;
  sound_data.streaming_data.in_use = true;
  return true;
}
bool PauseStreaming() {
  if (!sound_data.streaming_data.in_use) return false;
  WaitForSingleObject(sound_data.hmutex, 0);
  if (sound_data.streaming_data.wave_data.IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  StopWaveData(&sound_data.streaming_data.wave_data);
  sound_data.streaming_data.in_pause = true;
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool ContinueStreaming() {
  if (!sound_data.streaming_data.in_use) return false;
  WaitForSingleObject(sound_data.hmutex, 0);
  if (sound_data.streaming_data.wave_data.IsNull()) {
    ReleaseMutex(sound_data.hmutex);
    return false;
  }
  PlayWaveData(&sound_data.streaming_data.wave_data);
  sound_data.streaming_data.in_pause = false;
  ReleaseMutex(sound_data.hmutex);
  return true;
}
bool StopStreaming() {
  if (!sound_data.streaming_data.in_use) return false;
  WaitForSingleObject(sound_data.hmutex, 0);
  sound_data.streaming_data.stop_request = true;
  ReleaseMutex(sound_data.hmutex);
  //
  WaitForSingleObject(sound_data.streaming_data.hthread, INFINITE);
  CloseHandle(sound_data.streaming_data.hthread);
  ReleaseWaveData(&sound_data.streaming_data.wave_data);
  sound_data.streaming_data.Reset();
  return true;
}
////////////////////////////////////////
// Input
////////////////////////////////////////
bool SetVirtualInputKey(SYS_VIRTUAL_KEY virtual_key, SYS_KEY key,
                        SYS_JOYPAD_KEY joypad_key) {
  switch (virtual_key) {
    case SYS_VIRTUAL_KEY_DOWN:
    case SYS_VIRTUAL_KEY_LEFT:
    case SYS_VIRTUAL_KEY_RIGHT:
    case SYS_VIRTUAL_KEY_UP:
      ErrorDialogBox(SYS_ERROR_ARROW_KEYS_NOT_CUSTOM);
      return false;
    default:
      if ((virtual_key < 0) || (virtual_key >= SYS_ELEMNUM_BUTTON_KEY)) {
        ErrorDialogBox(SYS_ERROR_INVALID_VIRTUAL_KEY, virtual_key);
        return false;
      }
      // Key assign
      if ((key < 0) || (key >= SYS_ELEMNUM_KEYID)) {
        ErrorDialogBox(SYS_ERROR_INVALID_KEY, key);
        return false;
      }
      // Joypad assign
      if ((joypad_key < 0) || (joypad_key >= SYS_ELEMNUM_CONTROLLERID)) {
        ErrorDialogBox(SYS_ERROR_INVALID_JOYPAD_KEY, joypad_key);
        return false;
      }
      input_data.key_map.insert(std::make_pair(key, virtual_key));
      input_data.joypad_key_map.insert(
          std::make_pair(joypad_key, virtual_key));
      break;
  }
  return true;
}
void SetJoypadThreshold(int threshold_x, int threshold_y) {
  if ((threshold_x <= 0) || (threshold_x >= SYS_JOYPAD_RANGE_MAX)) {
    threshold_x = SYS_JOYPAD_THRESHOLD_DEFAULT;
    ErrorDialogBox(SYS_ERROR_INVALID_THRESHOLD, threshold_x);
  }
  input_data.joypad_status.threshold_x = threshold_x;
  if ((threshold_y <= 0) || (threshold_y >= SYS_JOYPAD_RANGE_MAX)) {
    threshold_y = SYS_JOYPAD_THRESHOLD_DEFAULT;
    ErrorDialogBox(SYS_ERROR_INVALID_THRESHOLD, threshold_y);
  }
  input_data.joypad_status.threshold_y = threshold_y;
}
bool GetKeyboardStatus(SYS_KEY key) {
  return input_data.keyboard_status.IsON(key);
}
bool GetJoypadStatus(SYS_JOYPAD_KEY joypad_key) {
  return input_data.joypad_status.IsON(joypad_key);
}
bool GetVirtualInputStatus(SYS_VIRTUAL_KEY virtual_key) {
  return input_data.virtual_status.IsON(virtual_key);
}
bool GetVirtualInputPressed(SYS_VIRTUAL_KEY virtual_key) {
  return input_data.virtual_pressed.IsON(virtual_key);
}
////////////////////////////////////////
// Debug
////////////////////////////////////////
bool ErrorDialogBox(const wchar_t* format, ...) {
  wchar_t buffer[256] = {0};
  va_list args;
  va_start(args, format);
  vswprintf_s(buffer, 256, format, args);
  MessageBox(system_data.hwnd, buffer, L"System Error", MB_ICONEXCLAMATION);
  return true;
}
}  // namespace sys
