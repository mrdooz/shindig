#include "stdafx.h"
#include "font.hpp"
#include <celsus/file_utils.hpp>

Font::Font()
	: _font_file(new FileReader())
	, _height(0)
	, _scale(0)
{
}

Font::~Font()
{
	SAFE_DELETE(_font_file);
}

bool Font::init(const char *filename, float height)
{
	RETURN_ON_FAIL_BOOL_E(_font_file->load(filename));
	ZeroMemory(&_font, sizeof(_font));
	if (stbtt_InitFont(&_font, _font_file->data(), stbtt_GetFontOffsetForIndex(_font_file->data(),0)) == 0)
		return false;
	_height = height;
	_scale = stbtt_ScaleForPixelHeight(&_font, _height);


	uint8_t *buf = new uint8_t[256*256*4];

	render("Magnus", buf, 256, 256);
	save_bmp32("c:/temp/tjong.bmp", buf, 256, 256);

	SAFE_ADELETE(buf);

	return true;
}


void Font::render(const char *text, uint8_t *ptr, int width, int height)
{
	ZeroMemory(ptr, width * height * 4);

	int x = 0;
	int y = 0;
	int max_height = 0;	// max height of a letter on the current row
	while (*text) {
		int new_lines = 0;
		while (*text == '\n') {
			new_lines++;
			text++;
		}
		char ch = *text;
		int w, h;
		int xoffs = 0;
		int yoffs = 0;
		uint8_t *bitmap = stbtt_GetCodepointBitmap(&_font, 0,_scale, ch, &w, &h, &xoffs, &yoffs);
		// check if we need to skip to the next line
		if (x + w > width)
			new_lines = 1;
		if (new_lines) {
			// check if it's possible..
			y += new_lines * (max_height != 0 ? max_height : (int)_height);
			if (y > height)
				break;
			x  = 0;
			max_height = 0;
		}

		// draw the char
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				// assume ptr contains 32 bit data
				int c = bitmap[j*w+i];
				int ofs = 4 * ((y+j) * width + x + i);
				ptr[ofs+0] = ptr[ofs+1] = ptr[ofs+2] = ptr[ofs+3] = c;
			}
		}
		x += w;
		//y += h;
		max_height = std::max<int>(max_height, h);
		++text;
	}
}
