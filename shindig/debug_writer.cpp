#include "stdafx.h"
#include "debug_writer.hpp"
#include "font.hpp"
#include "render_target.hpp"
#include "resource_manager.hpp"
#include "system.hpp"
#include "lua_utils.hpp"
#include <celsus/effect_wrapper.hpp>

DebugWriter::DebugWriter()
  : _effect(new EffectWrapper())
  , _top(-1)
  , _left(-1)
  , _width(-1)
  , _height(-1)
{
}

DebugWriter::~DebugWriter()
{
	SAFE_DELETE(_effect);
  container_delete(_fonts);
}

bool DebugWriter::init(const string2& font_name, int top, int left, int width, int height)
{
  _font_name = font_name;
  _top = top; 
  _left = left;
  _width = width; 
  _height = height;

	using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_writer2.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
		MakeDelegate(this, &DebugWriter::load_effect)));

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/debug_writer.lua", System::kDirRelative), MakeDelegate(this, &DebugWriter::load_states), true));

	return true;
}

DebugWriter::FontInstance *DebugWriter::find_font(float height)
{
  // finds an instance of the font with the given size, or creates a new one
  for (auto i = _fonts.begin(), e = _fonts.end(); i != e; ++i) {
    if ((*i)->height == height)
      return (*i);
  }

  // create new font
  Font *f = new Font();
  if (!f->init(_font_name, height))
    return NULL;
  _fonts.push_back(new FontInstance(_font_name, height, f));
  return _fonts.back();
}

void DebugWriter::render()
{
	auto context = Graphics::instance().context();

  // setup states shared between all fonts
  _effect->set_shaders(context);
  ID3D11SamplerState *samplers[] = { _sampler_state };
  context->PSSetSamplers(0, 1, samplers);

  float blend_factor[] = { 1, 1, 1, 1 };
  context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);
  context->OMSetDepthStencilState(_dss, 0xffffffff);

  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto i = _text_segments.begin(); i != _text_segments.end(); ++i) {

    Font *f = i->first->font;
    Verts *verts = &i->first->_verts;
    PosTex *v = verts->map();
    int chars = 0;
    const std::vector<TextSegment>& s = i->second;
    for (auto j = s.begin(), je = s.end(); j != je; ++j) {
      v = f->render(j->text, v, _width, _height, j->pos);
      chars += j->text.size();
    }

    verts->unmap();

    // draw current font
    ID3D11ShaderResourceView* t[] = { f->view()};
    context->PSSetShaderResources(0, 1, t);

    set_vb(context, verts->vb(), Verts::stride);
    context->Draw(6 * chars, 0);
}


}

void DebugWriter::reset_frame()
{
	_text_segments.clear();
}

void DebugWriter::calc_extents(int *width, int *height, float h, const char *fmt, ...)
{
	FontInstance *f = find_font(h);
	if (!f)
		return;

	va_list arg;
	va_start(arg, fmt);

	const int len = _vscprintf(fmt, arg) + 1;
	char* buf = (char*)_alloca(len);
	vsprintf_s(buf, len, fmt, arg);

	f->font->calc_extents(buf, _width, _height, width, height);
}

void DebugWriter::write(const int left, const int top, float h, const char *fmt, ...)
{
  FontInstance *f = find_font(h);
  if (!f)
    return;

  va_list arg;
  va_start(arg, fmt);

  const int len = _vscprintf(fmt, arg) + 1;
  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, fmt, arg);

  _text_segments[f].push_back(TextSegment(D3DXVECTOR3((float)left, (float)top, 0), string2(buf), h));
  va_end(arg);
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
