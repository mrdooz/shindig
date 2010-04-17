#include "stdafx.h"
#include "render_target.hpp"
#include "graphics.hpp"

RenderTarget::RenderTarget()
	: _width(-1)
	, _height(-1)
{
}

bool RenderTarget::create(const int width, const int height)
{
	_width = width;
	_height = height;
	ID3D11Device* device = Graphics::instance().device();
	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 0, D3D11_BIND_RENDER_TARGET);
	RETURN_ON_FAIL_BOOL(device->CreateTexture2D(&desc, NULL, &_render_target), ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(device->CreateRenderTargetView(_render_target, NULL, &_render_target_view), ErrorPredicate<HRESULT>, LOG_ERROR_LN);

	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  desc.MipLevels = 1;
	RETURN_ON_FAIL_BOOL(device->CreateTexture2D(&desc, NULL, &_depth_stencil), ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(device->CreateDepthStencilView(_depth_stencil, NULL, &_depth_stencil_view), ErrorPredicate<HRESULT>, LOG_ERROR_LN);

	_viewport = CD3D11_VIEWPORT (0.0f, 0.0f, (float)_width, (float)_height);

	return true;
}

void RenderTarget::set()
{
	ID3D11DeviceContext* context = Graphics::instance().context();
	ID3D11RenderTargetView* render_targets[] = { _render_target_view };
	context->OMSetRenderTargets(1, render_targets, _depth_stencil_view);
	context->RSSetViewports(1, &_viewport);
}
