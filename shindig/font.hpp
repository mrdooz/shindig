#pragma once
#include "stb_truetype.h"
#include <celsus/vertex_types.hpp>
#include <celsus/refptr.hpp>

class FileReader;

struct FontInfo
{
  // 0, 1
  // 2, 3
	D3DXVECTOR2 _uv[4];
	int _w, _h;
	int _ofsx, _ofsy;
};

// wrapper around stb-truetype
class Font
{
public:
	Font();
	~Font();
	bool init(const char *filename, float font_height);
  PosTex *render(const char *text, PosTex *vtx, int width, int height, const D3DXVECTOR3& ofs);
  ID3D11ShaderResourceView *view() const { return _view; }
private:
	bool pack_font();
	float _font_height;
	int _texture_width;
	int _texture_height;
	float _scale;
	RefPtr<FileReader> _font_file;
	stbtt_fontinfo _font;
  CComPtr<ID3D11Texture2D> _texture;
  CComPtr<ID3D11ShaderResourceView> _view;

	typedef std::map<int, FontInfo>	FontMap;
	FontMap _font_map;
};
