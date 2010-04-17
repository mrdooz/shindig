#include "stdafx.h"
#include "graphics.hpp"
#include "system.hpp"

Graphics* Graphics::_instance = NULL;

Graphics& Graphics::instance()
{
	if (_instance == NULL) {
		_instance = new Graphics();
	}

	return *_instance;
}

Graphics::Graphics()
	: _width(-1)
	, _height(-1)
{
}

Graphics::~Graphics()
{
}

bool Graphics::init(const HWND hwnd, const int width, const int height)
{
	if (!init_directx(hwnd, width, height)) {
		return false;
	}

	_con = System::instance().add_file_changed(fastdelegate::bind(&Graphics::file_changed, this, _1));

	return true;
}

bool Graphics::init_directx(const HWND hwnd, const int width, const int height)
{
	_width = width;
	_height = height;
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

	RETURN_ON_FAIL_BOOL(D3D11CreateDeviceAndSwapChain(
		NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &sd, &_swap_chain, &_device, &feature_level, &_immediate_context),
    ErrorPredicate<HRESULT>, LOG_ERROR_LN);

	if (feature_level < D3D_FEATURE_LEVEL_9_3) {
		return false;
	}

	CComPtr<ID3D11Texture2D> back_buffer;
	RETURN_ON_FAIL_BOOL(_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)), ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(_device->CreateRenderTargetView(back_buffer, NULL, &_render_target_view), ErrorPredicate<HRESULT>, LOG_ERROR_LN);

	// depth buffer
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	RETURN_ON_FAIL_BOOL(_device->CreateTexture2D(&depthBufferDesc, NULL, &_depth_stencil), ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(_device->CreateDepthStencilView(_depth_stencil, NULL, &_depth_stencil_view), ErrorPredicate<HRESULT>, LOG_ERROR_LN);

	_viewport = CD3D11_VIEWPORT (0.0f, 0.0f, (float)_width, (float)_height);
	set_default_render_target();

	return true;
}

void Graphics::set_default_render_target()
{
	ID3D11RenderTargetView* render_targets[] = { _render_target_view };
	_immediate_context->OMSetRenderTargets(1, render_targets, _depth_stencil_view);
	_immediate_context->RSSetViewports(1, &_viewport);
}

bool Graphics::close()
{
	if (!close_directx()) {
		return false;
	}

	_con.disconnect();
	return true;
}

bool Graphics::close_directx()
{
	_depth_stencil_view.Release();
	_depth_stencil.Release();
	_render_target_view.Release();
	_immediate_context.Release();
	_swap_chain.Release();
	_device.Release();
	return true;
}

void Graphics::clear()
{
	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	_immediate_context->ClearRenderTargetView(_render_target_view, ClearColor);
	_immediate_context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}

void Graphics::present()
{
	_swap_chain->Present(0,0);
}

void Graphics::resize(const int width, const int height)
{
	if (_swap_chain) {
		_swap_chain->ResizeBuffers(1, width, height, _buffer_format, 0);
	}
}

void Graphics::file_changed(const std::string& filename)
{
	int a = 10;
}
