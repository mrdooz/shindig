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
	ID3D11RenderTargetView* render_targets[] = { _render_target_view };
	_immediate_context->OMSetRenderTargets(1, render_targets, NULL);

	CD3D11_VIEWPORT vp((float)width, (float)height, 0, 1, 0, 0);
	_immediate_context->RSSetViewports(1, &vp);
/*
	EffectWrapper e(_device);
	e.load_states("/projects/shindig/effects/states.fx");
	e.load("/projects/shindig/effects/systemVS.fx");
*/
	return true;
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
	_render_target_view.Release();
	_immediate_context.Release();
	_swap_chain.Release();
	_device.Release();
	return true;
}

void Graphics::tick()
{
	float ClearColor[4] = { 0, 1.0f, 1.0f, 1.0f };
	_immediate_context->ClearRenderTargetView(_render_target_view, ClearColor);
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
