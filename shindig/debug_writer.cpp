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

	return true;
}

void DebugWriter::render()
{
	auto context = Graphics::instance().context();
	D3D11_MAPPED_SUBRESOURCE s;
	context->Map(_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &s);
	_font->render(_text, (uint8_t *)s.pData, _width, _height);
	int a = 10;
	context->Unmap(_texture, 0);

	_effect->set_shaders(context);
	//context->IASetInputLayout(_particle_layout);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//set_vb(context, _particle_vb.vb(), sizeof(ParticleVb));
/*
	ID3D11ShaderResourceView* t[] = { _texture };
	ID3D11SamplerState *samplers[] = { _sampler_state };
	float blend_factors[4] = {1, 1, 1, 1};
	context->OMSetBlendState(_blend_state, blend_factors, 0xffffffff);
	context->OMSetDepthStencilState(_particle_dss, 0xffffffff);
	context->PSSetSamplers(0, 1, samplers);
	context->PSSetShaderResources(0, 1, t);
	*/
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
