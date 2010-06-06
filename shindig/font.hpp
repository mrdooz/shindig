#pragma once
#include "stb_truetype.h"

class FileReader;

// wrapper around stb-truetype
class Font
{
public:
	Font();
	~Font();
	bool init(const char *filename, float height);
	void render(const char *text, uint8_t *ptr, int width, int height);
private:
	float _height;
	float _scale;
	FileReader *_font_file;
	stbtt_fontinfo _font;

};
