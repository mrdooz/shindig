#include "stdafx.h"
#include "debug_writer.hpp"
#include "font.hpp"
#include "render_target.hpp"
#include "system.hpp"
#include <celsus/effect_wrapper.hpp>

DebugWriter::DebugWriter()
	: _width(0)
	, _height(0)
	, _font(new Font())
	, _effect(new EffectWrapper())
{
}

DebugWriter::~DebugWriter()
{
	SAFE_DELETE(_effect);
	SAFE_DELETE(_font);
}

bool DebugWriter::init(int width, int height)
{
	RETURN_ON_FAIL_BOOL_E(_font->init("c:/windows/fonts/arialbd.ttf", 20));
	_width = width;
	_height = height;

	ID3D11Device* device = Graphics::instance().device();
	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	RETURN_ON_FAIL_BOOL_E(device->CreateTexture2D(&desc, NULL, &_texture));
	RETURN_ON_FAIL_BOOL_E(device->CreateShaderResourceView(_texture, NULL, &_view));

	System& sys = System::instance();
	RETURN_ON_FAIL_BOOL_E(_effect->load_shaders(sys.convert_path("effects/debug_writer.fx", System::kDirRelative), "vsMain", NULL, "psMain"));

  using namespace rt;
  _sampler_state.Attach(D3D11::SamplerDescription().
    AddressU_(D3D11_TEXTURE_ADDRESS_CLAMP).
    AddressV_(D3D11_TEXTURE_ADDRESS_CLAMP).
    Filter_(D3D11_FILTER_MIN_MAG_MIP_LINEAR).
    Create(device));

  _blend_state.Attach(D3D11::BlendDescription().
    RenderTarget_(0, D3D11::RenderTargetBlendDescription().
    BlendEnable_(TRUE).
    BlendOp_(D3D11_BLEND_OP_ADD).
    BlendOpAlpha_(D3D11_BLEND_OP_ADD).
    SrcBlend_(D3D11_BLEND_ONE).
    DestBlend_(D3D11_BLEND_ZERO).
    SrcBlendAlpha_(D3D11_BLEND_SRC_ALPHA).
    DestBlendAlpha_(D3D11_BLEND_INV_SRC_ALPHA)).
    Create(device));


	return true;
}

void DebugWriter::render()
{
	auto context = Graphics::instance().context();
	D3D11_MAPPED_SUBRESOURCE s;
	context->Map(_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &s);
	_font->render(_text, (uint8_t *)s.pData, _width, _height);
	context->Unmap(_texture, 0);

	_effect->set_shaders(context);
  ID3D11SamplerState *samplers[] = { _sampler_state };
  context->PSSetSamplers(0, 1, samplers);
  ID3D11ShaderResourceView* t[] = { _view };
  context->PSSetShaderResources(0, 1, t);

  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
}

void DebugWriter::reset_frame()
{
	_text.clear();
}

void DebugWriter::write(const char *msg)
{
	_text += msg;
}

void DebugWriter::close()
{

}
