#pragma once
#include "dynamic_vb.hpp"
#include "font.hpp"
#include <celsus/vertex_types.hpp>

class RenderTarget;
class EffectWrapper;

// A debug writer write with a specific font to the area defined by top/left, width/height

class DebugWriter
{
public:
	DebugWriter();
	~DebugWriter();
	bool init(const string2& font_name, int top, int left, int width, int height);
	void close();
	void render();
	void reset_frame();
	void write(const int left, const int top, float h, const char *fmt, ...);

private:

  typedef DynamicVb<PosTex> Verts;

  struct FontInstance
  {
    FontInstance(const string2& name, float h, Font *f) : font_name(name), height(h), font(f) { _verts.create(10000); }
    ~FontInstance() { delete font; }
    string2 font_name;
    float height;
    Font* font;
    Verts _verts;
  };

  struct TextSegment
  {
    TextSegment(const D3DXVECTOR3& pos, const string2& text, float h) : pos(pos), text(text), h(h) {}
    D3DXVECTOR3 pos;
    string2 text;
    float h;
  };


	void load_effect(EffectWrapper *effect);
	bool load_states(const string2& filename);

  FontInstance *find_font(float height);

  typedef std::vector<FontInstance*> Fonts;

  // text segments are stored per font instance
	typedef std::map<FontInstance*, std::vector<TextSegment> > TextSegments;
	TextSegments _text_segments;
  string2 _font_name;
  int _top, _left;
	int _width, _height;
  Fonts _fonts;
  CComPtr<ID3D11SamplerState> _sampler_state;
  CComPtr<ID3D11BlendState> _blend_state;
  CComPtr<ID3D11InputLayout> _layout;
	CComPtr<ID3D11DepthStencilState> _dss;
	EffectWrapper *_effect;
};