#pragma once
#include "stb_truetype.h"
#include <celsus/vertex_types.hpp>

class FileReader;

struct FontInfo
{
  // 0, 1
  // 2, 3
	D3DXVECTOR2 _uv[4];
	int _w, _h;
};


// wrapper around stb-truetype
class Font
{
public:
	Font();
	~Font();
	bool init(const char *filename, float height);
  void render(const char *text, uint8_t *ptr, int width, int height);
  void render2(const char *text, PosTex *vtx, int width, int height);

  ID3D11ShaderResourceView *view() const { return _view; }
private:
	bool pack_font();
	float _height;
	float _scale;
	FileReader *_font_file;
	stbtt_fontinfo _font;
  CComPtr<ID3D11Texture2D> _texture;
  CComPtr<ID3D11ShaderResourceView> _view;

	typedef std::map<int, FontInfo>	FontMap;
	FontMap _font_map;
};
