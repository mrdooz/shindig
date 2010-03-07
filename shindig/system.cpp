#include "stdafx.h"
#include "system.hpp"
#include "redux_loader.hpp"
#include "effect_wrapper.hpp"

// http://www.directx11tutorials.com/category/tutorials/getting-started

ID3D11Device* g_d3d_device = NULL;

System::System()
{
}

System::~System()
{
}

bool System::init_directx(const HWND hwnd, const int width, const int height)
{
  _buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;

  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory( &sd, sizeof( sd ) );
  sd.BufferCount = 1;
  sd.BufferDesc.Width = width;
  sd.BufferDesc.Height = height;
  sd.BufferDesc.Format = _buffer_format;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hwnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;

  const int flags = D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL feature_level;

  RETURN_ON_FAIL_HR_BOOL(D3D11CreateDeviceAndSwapChain(
    NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &sd, &_swap_chain, &_device, &feature_level, &_immediate_context));

  if (feature_level < D3D_FEATURE_LEVEL_9_3) {
    return false;
  }

	g_d3d_device = _device;

  CComPtr<ID3D11Texture2D> back_buffer;
  RETURN_ON_FAIL_HR_BOOL(_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)));
  RETURN_ON_FAIL_HR_BOOL(_device->CreateRenderTargetView(back_buffer, NULL, &_render_target_view));
  ID3D11RenderTargetView* render_targets[] = { _render_target_view };
  _immediate_context->OMSetRenderTargets(1, render_targets, NULL);

  CD3D11_VIEWPORT vp((float)width, (float)height, 0, 1, 0, 0);
  _immediate_context->RSSetViewports(1, &vp);

	EffectWrapper e(_device);
	e.load("/projects/shindig/effects/systemVS.fx");

/*
	char user_dir[MAX_PATH];
	GetEnvironmentVariable("USERPROFILE", user_dir, MAX_PATH);
	char full_path[MAX_PATH];
	sprintf(full_path, "%s/My Documents/My Dropbox/data/scenes/diskette.rdx", user_dir);

	ReduxLoader loader(full_path, NULL, NULL, NULL);
	loader.load();

  json_test();
*/
  return true;
}

void System::tick()
{
  float ClearColor[4] = { 0, 1.0f, 1.0f, 1.0f };
  _immediate_context->ClearRenderTargetView(_render_target_view, ClearColor);
  _swap_chain->Present(0,0);
}

void System::resize(const int width, const int height)
{
  if (_swap_chain) {
    _swap_chain->ResizeBuffers(1, width, height, _buffer_format, 0);
  }
}
