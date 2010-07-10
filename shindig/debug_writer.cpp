#include "stdafx.h"
#include "debug_writer.hpp"
#include "font.hpp"
#include "render_target.hpp"
#include "resource_manager.hpp"
#include "system.hpp"
#include "lua_utils.hpp"
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

bool DebugWriter::init(int width, int height, float font_height)
{
	using namespace fastdelegate;
	using namespace rt;

	auto& sys = System::instance();

	_font_height = font_height;
	RETURN_ON_FAIL_BOOL_E(_font->init(sys.convert_path("data/fonts/arialbd.ttf", System::kDirRelative), font_height, width, height));
	_width = width;
	_height = height;

	ID3D11Device* device = Graphics::instance().device();
	CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_writer2.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
		MakeDelegate(this, &DebugWriter::load_effect)));

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/debug_writer.lua", System::kDirRelative), MakeDelegate(this, &DebugWriter::load_states), true));

  _verts.create(10000);

	return true;
}

void DebugWriter::render()
{
	auto context = Graphics::instance().context();
  PosTex *v = _verts.map();
	int chars = 0;
	for (auto i = _text.begin(); i != _text.end(); ++i) {
		v = _font->render(i->text, v, _width, _height, i->w, i->h, i->pos);
		chars += i->text.size();
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
	context->Draw(6 * chars, 0);
}

void DebugWriter::reset_frame()
{
	_text.clear();
}

void DebugWriter::write(const int left, const int top, const char *msg, const float w, const float h)
{
	// pos specifies top-left corner in pixel coordinates
	_text.push_back(TextSegment(D3DXVECTOR3((float)left, (float)top, 0), msg, w, h));
}

void DebugWriter::close()
{

}

void DebugWriter::load_effect(EffectWrapper *effect)
{
	SAFE_DELETE(_effect);
	_effect = effect;

	InputDesc().
		add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0).
		create(_layout, _effect);
}

bool DebugWriter::load_states(const string2& filename)
{
	auto& s = System::instance();
	if (!::load_states(filename, "default_blend", "default_dss", "default_sampler", &_blend_state.p, &_dss.p, &_sampler_state.p))
		return false;

	return true;
}
