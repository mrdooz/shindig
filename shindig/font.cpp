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
		while (*text == '\n' && *text != NULL) {
			new_lines++;
			text++;
		}
		char ch = *text;
    if (ch == 0)
      break;
		int w, h;
		int xoffs = 0;
		int yoffs = 0;
/*
    extern void stbtt_MakeCodepointBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint);
    // the same as above, but you pass in storage for the bitmap in the form
    // of 'output', with row spacing of 'out_stride' bytes. the bitmap is
    // clipped to out_w/out_h bytes. call the next function to get the
    // height and width and positioning info

    extern void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
    */
    int ix0, ix1, iy0, iy1;
    stbtt_GetCodepointBitmapBox(&_font, ch, _scale, _scale, &ix0, &iy0, &ix1, &iy1);
    w = ix1 - ix0;

		//uint8_t *bitmap = stbtt_GetCodepointBitmap(&_font, 0,_scale, ch, &w, &h, &xoffs, &yoffs);
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

    stbtt_MakeCodepointBitmap(&_font, ptr + 4 * (y*width+x), ix1-ix0, iy1-iy0, 4*width, _scale, _scale, ch);
      //const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint);
/*
		// draw the char
		for (int j = 0; j < h; ++j) {
			for (int i = 0; i < w; ++i) {
				// assume ptr contains 32 bit data
				int c = bitmap[j*w+i];
				int ofs = 4 * ((y+j) * width + x + i);
				ptr[ofs+0] = ptr[ofs+1] = ptr[ofs+2] = ptr[ofs+3] = c;
			}
		}
*/
		x += w;
		//y += h;
		max_height = std::max<int>(max_height, h);
		++text;
	}
}
