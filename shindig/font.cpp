#include "stdafx.h"
#include "font.hpp"
#include <celsus/file_utils.hpp>

// This is an implementation of http://www.blackpawn.com/texts/lightmaps/default.html
struct PixelRect
{
	// from top/left inclusive to bottom/right non-inclusive
	PixelRect() : _top(0), _left(0), _bottom(0), _right(0) {}
	PixelRect(int top, int left, int bottom, int right) : _top(top), _left(left), _bottom(bottom), _right(right) {}
	int width() const { return _right - _left; }
	int height() const { return _bottom - _top; }

	int	_top, _left, _bottom, _right;
};

struct Image
{
	Image() : _bitmap(nullptr) {}
	const char *_bitmap;
	PixelRect _rc;
};

struct Node
{
	Node()
	{
		_children[0] = _children[1] = nullptr;
		_image = nullptr;
	}

	bool is_leaf() const { return _children[0] == nullptr && _children[1] == nullptr; }

	Node *insert(const Image *img);

	Node *_children[2];
	PixelRect _rc;
	Image *_image;
};

Node *Node::insert(const Image *img)
{
	if (!is_leaf()) {
		if (Node *n = _children[0]->insert(img))
			return n;
		return _children[1]->insert(img);
	}	else {
		// return if node is already occupied
		if (_image)
			return nullptr;

		// return if img doesn't fit here
		int dx = _rc.width() - img->_rc.width(), dy = _rc.height() - img->_rc.height();
		if (dx < 0 || dy < 0)
			return nullptr;

		// return if perfect fit
		if (dx == 0 && dy == 0)
			return this;

		// create 2 children
		_children[0] = new Node();
		_children[1] = new Node();
		if (dx >= dy) {
			_children[0]->_rc = PixelRect(_rc._top, _rc._left, _rc._bottom, _rc._left+img->_rc.width());
			_children[1]->_rc = PixelRect(_rc._top, _rc._left+img->_rc.width(), _rc._bottom, _rc._right);
		} else {
			_children[0]->_rc = PixelRect(_rc._top, _rc._left, _rc._top+img->_rc.height(), _rc._right);
			_children[1]->_rc = PixelRect(_rc._top+img->_rc.height(), _rc._left, _rc._bottom, _rc._right);
		}
		return _children[0]->insert(img);
	}	
	return nullptr;
}

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
	pack_font();
	return true;
}

void Font::pack_font()
{
	Node *root = new Node();
	uint8_t *buf = new uint8_t[256*256*4];
	root->_rc = PixelRect(0, 0, 256, 256);

	char char_map[256];
	for (int i = 1; i < 256; ++i) {
		char_map[i-1] = (char)i;
	}
	char_map[255] = 0;
//	char char_map[] = "abcdefghijklmnopqrstuvwxyzåäö";
	char *cur = char_map;
	while (*cur) {
		int w, h;
		uint8_t *bitmap = stbtt_GetCodepointBitmap(&_font, 0,_scale, *cur, &w, &h, NULL, NULL);
		Image *img = new Image();
		img->_rc = PixelRect(0, 0, h, w);
		if (Node *n = root->insert(img)) {
			n->_image = img;
			int x = n->_rc._left, y = n->_rc._top;
			for (int j = 0; j < h; ++j) {
				for (int i = 0; i < w; ++i) {
					// assume ptr contains 32 bit data
					int c = bitmap[j*w+i];
					int ofs = 4 * ((y+j) * 256 + x + i);
					buf[ofs+0] = buf[ofs+1] = buf[ofs+2] = buf[ofs+3] = c;
				}
			}

		} else {
			SAFE_DELETE(img);
		}
		++cur;
	}

	save_bmp32("c:/temp/tjong.bmp", buf, 256, 256);
	SAFE_ADELETE(buf);

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
				int ofs = 4 * ((y+j+20+yoffs) * width + x + i);
				ptr[ofs+0] = ptr[ofs+1] = ptr[ofs+2] = ptr[ofs+3] = c;
			}
		}

		x += w;
		//y += h;
		max_height = std::max<int>(max_height, h);
		++text;
	}
}
