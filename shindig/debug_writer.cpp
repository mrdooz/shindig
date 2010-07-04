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
	RETURN_ON_FAIL_BOOL_E(_effect->load_shaders(sys.convert_path("effects/debug_writer2.fx", System::kDirRelative), "vsMain", NULL, "psMain"));

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
    SrcBlend_(D3D11_BLEND_SRC_COLOR).
    DestBlend_(D3D11_BLEND_DEST_COLOR).
    SrcBlendAlpha_(D3D11_BLEND_SRC_ALPHA).
    DestBlendAlpha_(D3D11_BLEND_INV_SRC_ALPHA)).
    Create(device));
		
	_dss.Attach(D3D11::DepthStencilDescription().DepthEnable_(FALSE).Create(device));

  InputDesc().
    add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
    add("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0).
    create(_layout, _effect);


  _verts.create(10000);

	return true;
}

void DebugWriter::render()
{
	auto context = Graphics::instance().context();
  PosTex *v = _verts.map();
	for (auto i = _text.begin(); i != _text.end(); ++i) {
		_font->render(i->text, v, _width, _height, i->pos);
	}
  _verts.unmap();

	_effect->set_shaders(context);
  ID3D11SamplerState *samplers[] = { _sampler_state };
  context->PSSetSamplers(0, 1, samplers);
  ID3D11ShaderResourceView* t[] = { _font->view()};
  context->PSSetShaderResources(0, 1, t);

  float blend_factor[] = { 1, 1, 1, 1 };
  context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);
	context->OMSetDepthStencilState(_dss, 0xffffffff);

  set_vb(context, _verts.vb(), Verts::stride);
  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(6 * _text.size(), 0);
}

void DebugWriter::reset_frame()
{
	_text.clear();
}

void DebugWriter::write(const int top, const int left, const char *msg)
{
	// pos specifies top-left corner in pixel coordinates
	_text.push_back(TextSegment(D3DXVECTOR3(left, top, 0), msg));
}

void DebugWriter::close()
{

}
