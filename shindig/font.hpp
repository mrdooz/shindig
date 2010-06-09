#pragma once
#include "stb_truetype.h"

class FileReader;

struct FontInfo
{
	D3DXVECTOR2 _uv;
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
private:
	void pack_font();
	float _height;
	float _scale;
	FileReader *_font_file;
	stbtt_fontinfo _font;

	typedef std::map<int, FontInfo>	FontMap;
	FontMap _font_map;
};
