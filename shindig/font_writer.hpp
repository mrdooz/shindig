#pragma once

class RenderTarget;
class EffectWrapper;

class Font;
struct FontInstance;

// A FontWriter writes with a specific font to the area defined by top/left, width/height
class FontWriter
{
public:
	FontWriter();
	~FontWriter();
	bool init(const string2& font_name, int top, int left, int width, int height);
	void close();
	void render();
	void reset_frame();
	void write(const int left, const int top, float h, const char *fmt, ...);
	void calc_extents(int *width, int *height, float h, const char *fmt, ...);

	enum AlignFlags {
		kAlignLeft				= 1 << 0,
		kAlightRight			= 1 << 1,
		kAlignHorizCenter = 1 << 2,
		kAlignTop					= 1 << 3,
		kAlignBottom			= 1 << 4,
		kAlignVertCenter	= 1 << 5,
	};

private:
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
	std::auto_ptr<EffectWrapper> _effect;
};
