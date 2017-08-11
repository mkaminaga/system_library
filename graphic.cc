  // @file graphic
  // @brief Definitions of graphic related structures and functions.
  // @author Mamoru Kaminaga
  // @date 2017-07-27 21:04:42
  // Copyright 2017 Mamoru Kaminaga
#include <assert.h>
#include <unordered_map>
#include <vector>
#include "./graphic.h"
#include "./graphic_internal.h"
#include "./system_internal.h"
namespace sys {
  //
  // These are public structures related to graphic
  //
TextureDesc::TextureDesc() : resource_desc() { }
ImageDesc::ImageDesc() : texture_id(0), x(0), y(0), w(0), h(0), s(1.0),
    image_mode(SYS_IMAGEMODE_DEFAULT) { }
FontDesc::FontDesc() : resource_desc(), s(1.0),
    image_mode(SYS_IMAGEMODE_DEFAULT) { }

  //
  // These are internal structures related to graphic
  //
GraphicData graphic_data;
GraphicData::GraphicData() : device(nullptr), device_context(nullptr),
    dxgi_device(nullptr), dxgi_adaptor(nullptr), dxgi_factory(nullptr),
    dxgi_swap_chain(nullptr), back_buffer(nullptr),
    render_target_view(nullptr), input_layout(nullptr), vs_cbuffer(nullptr),
    ps_cbuffer(nullptr), blend_state(nullptr), sampler_state(nullptr),
    vertex_shader1(nullptr), pixel_shader1(nullptr), resolution(640, 480),
    on_fullscreen_start(false), on_power_save(false) {
  // The font table is initialized.
  const wchar_t font_array[SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM] =
    L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\] ";
  for (int i = 0; i < SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM; ++i) {
    font_map[font_array[i]] = i;
  }
  // The resource buffers are initialized.
  texture_buffer.resize(32);
  image_buffer.resize(32);
  font_buffer.resize(4);
}

  //
  // These are public structures related to graphic
  //
TextureData::TextureData() : w(0), h(0), blend_factor(),
    shader_resource_view() { }
void TextureData::Release() {
  SYS_SAFE_RELEASE(shader_resource_view[0]);
}
bool TextureData::IsNull() {
  return (shader_resource_view[0] == nullptr);
}
ImageData::ImageData() : w(0), h(0), texture_id(0), buffer(nullptr) { }
void ImageData::Release() {
  SYS_SAFE_RELEASE(buffer);
}
bool ImageData::IsNull() {
  return (buffer == nullptr);
}
FontData::FontData() : font_texture(), font_image() { }
void FontData::Release() {
  for (int i = 0; i < SYS_FONT_COLUMN_NUM * SYS_FONT_ROW_NUM; ++i) {
    font_image[i].Release();
  }
  font_texture.Release();
}
bool FontData::IsNull() {
  return font_texture.IsNull();
}

  //
  // These are private functions related to common
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
  // Check list item number.
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
    case DXGI_ERROR_DEVICE_RESET:
    case DXGI_ERROR_DEVICE_REMOVED:
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
    case DXGI_ERROR_INVALID_CALL:
      ErrorDialogBox(SYS_ERROR_DIRECTX_ERROR);
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
      // Full screen mode.
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
      ErrorDialogBox(SYS_ERROR_DIRECTX_ERROR);
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
    // Texture properties acquired.
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
  // Image polygon rectangle size in window set.
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
  // Coordinates from pixel to U-V.
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
  // Coordinates are texture polygon.
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
  Vector2d font_size;
  GetFontSize(font, &font_size);
  int text_length = wcslen(text);
  ImageData* font_image = nullptr;
  for (int i = 0; i < text_length; ++i) {
    if (graphic_data.font_map.count(text[i]) == 0) {
      font_image = &font->font_image[0];  // space ' '
    } else {
      font_image = &font->font_image[graphic_data.font_map[text[i]]];
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

  //
  // These are public functions related to graphic
  //
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
bool CreateTexture(const TextureDesc& desc, int* texture_id) {
  // 1. The id allocation is checked.
  int id = *texture_id = graphic_data.texture_id_server.CreateId();
  if (id == SYS_ID_SERVER_EXCEEDS_LIMIT) {
    ErrorDialogBox(SYS_ERROR_TEXTURE_ID_EXCEEDS_LIMIT, id);
    return false;
  }
  // 2. The buffer size is checked.
  if (id >= static_cast<int>(graphic_data.texture_buffer.size())) {
    // The buffer is expanded.
    graphic_data.texture_buffer.resize(graphic_data.texture_buffer.size() * 2);
  }
  return CreateTextureData(desc, &graphic_data.texture_buffer[id]);
}
bool ReleaseTexture(int texture_id) {
  // 1. The buffer size is checked.
  if (texture_id >= static_cast<int>(graphic_data.texture_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_TEXTURE_ID, texture_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.texture_buffer[texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, texture_id);
    return false;
  }
  graphic_data.texture_id_server.ReleaseId(texture_id);
  return ReleaseTextureData(&graphic_data.texture_buffer[texture_id]);
}
bool GetTextureSize(int texture_id, Vector2d* size) {
  // 1. The buffer size is checked.
  if (texture_id >= static_cast<int>(graphic_data.texture_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_TEXTURE_ID, texture_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.texture_buffer[texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, texture_id);
    return false;
  }
  return GetTextureSize(&graphic_data.texture_buffer[texture_id], size);
}
bool CreateImage(const ImageDesc& desc, int* image_id) {
  // 1. The id allocation is checked.
  int id = *image_id = graphic_data.image_id_server.CreateId();
  if (id == SYS_ID_SERVER_EXCEEDS_LIMIT) {
    ErrorDialogBox(SYS_ERROR_IMAGE_ID_EXCEEDS_LIMIT, id);
    return false;
  }
  // 2. The buffer size is checked.
  if (id >= static_cast<int>(graphic_data.image_buffer.size())) {
    // The buffer is expanded.
    graphic_data.image_buffer.resize(graphic_data.image_buffer.size() * 2);
  }
  // 3. Null check for texture id.
  if (graphic_data.texture_buffer[desc.texture_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, desc.texture_id);
    return false;
  }
  return CreateImageData(desc, &graphic_data.texture_buffer[desc.texture_id],
                         &graphic_data.image_buffer[id]);
}
bool ReleaseImage(int image_id) {
  // 1. The buffer size is checked.
  if (image_id >= static_cast<int>(graphic_data.image_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_IMAGE_ID, image_id);
    return false;
  }
  graphic_data.image_id_server.ReleaseId(image_id);
  return ReleaseImageData(&graphic_data.image_buffer[image_id]);
}
bool GetImageSize(int image_id, Vector2d* size) {
  // 1. The buffer size is checked.
  if (image_id >= static_cast<int>(graphic_data.image_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_IMAGE_ID, image_id);
    return false;
  }
  return GetImageSize(&graphic_data.image_buffer[image_id], size);
}
bool DrawImage(int image_id, const Vector2d& position, const Color4b& color) {
  // 1. The buffer size is checked.
  if (image_id >= static_cast<int>(graphic_data.image_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_IMAGE_ID, image_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.image_buffer[image_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_IMAGE_ID, image_id);
    return false;
  }
  // 3. Null check for texture.
  const int texture_id = graphic_data.image_buffer[image_id].texture_id;
  TextureData* texture = &graphic_data.texture_buffer[texture_id];
  if (texture->IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_TEXTURE_ID, texture_id);
    return false;
  }
  return DrawImageData(texture, &graphic_data.image_buffer[image_id], position,
                       color);
}
bool CreateFont(const FontDesc& desc, int* font_id) {
  // 1. The id allocation is checked.
  int id = *font_id = graphic_data.font_id_server.CreateId();
  if (id == SYS_ID_SERVER_EXCEEDS_LIMIT) {
    ErrorDialogBox(SYS_ERROR_FONT_ID_EXCEEDS_LIMIT, id);
    return false;
  }
  // 2. The buffer size is checked.
  if (id >= static_cast<int>(graphic_data.font_buffer.size())) {
    // The buffer is expanded.
    graphic_data.font_buffer.resize(graphic_data.font_buffer.size() * 2);
  }
  // 3. Duplicate check.
  if (!graphic_data.font_buffer[id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_DUPLICATE_FONT_ID, id);
    return false;
  }
  return CreateFontData(desc, &graphic_data.font_buffer[id]);
}
bool ReleaseFont(int font_id) {
  // 1. The buffer size is checked.
  if (font_id >= static_cast<int>(graphic_data.font_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_FONT_ID, font_id);
    return false;
  }
  graphic_data.font_id_server.ReleaseId(font_id);
  return ReleaseFontData(&graphic_data.font_buffer[font_id]);
}
bool GetFontSize(int font_id, Vector2d* size) {
  // 1. The buffer size is checked.
  if (font_id >= static_cast<int>(graphic_data.font_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check.
  if (graphic_data.font_buffer[font_id].IsNull()) {
    ErrorDialogBox(SYS_ERROR_NULL_FONT_ID, font_id);
    return false;
  }
  return GetFontSize(&graphic_data.font_buffer[font_id], size);
}
bool GetTextSize(int font_id, Vector2d* size, const wchar_t* format, ...) {
  // 1. The buffer size is checked.
  if (font_id >= static_cast<int>(graphic_data.font_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check.
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
  // 1. The buffer size is checked.
  if (font_id >= static_cast<int>(graphic_data.font_buffer.size())) {
    ErrorDialogBox(SYS_ERROR_INVALID_FONT_ID, font_id);
    return false;
  }
  // 2. Null check.
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
}  // namespace sys
