#pragma once
#include "dynamic_vb.hpp"
#include <celsus/vertex_types.hpp>


class Font;
class RenderTarget;
class EffectWrapper;

class DebugWriter
{
public:
	DebugWriter();
	~DebugWriter();
	bool init(int width, int height, float font_height);
	void close();
	void render();
	void reset_frame();
	void write(const int left, const int top, const char *msg);

private:
	struct TextSegment
	{
		TextSegment(const D3DXVECTOR3& pos, const string2& text) : pos(pos), text(text) {}
		D3DXVECTOR3 pos;
		string2 text;
	};

	void load_effect(EffectWrapper *effect);
	bool load_states(const string2& filename);

	typedef std::vector<TextSegment> Text;
	Text _text;
	int _width;
	int _height;
	float _font_height;
	Font *_font;
  CComPtr<ID3D11SamplerState> _sampler_state;
  CComPtr<ID3D11BlendState> _blend_state;
  CComPtr<ID3D11InputLayout> _layout;
	CComPtr<ID3D11DepthStencilState> _dss;
  typedef DynamicVb<PosTex> Verts;
  Verts _verts;
	EffectWrapper *_effect;
};