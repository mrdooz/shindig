#include "stdafx.h"
#include "font_writer.hpp"
#include "stb_truetype.h"
#include "render_target.hpp"
#include "resource_manager.hpp"
#include "system.hpp"
#include "lua_utils.hpp"
#include <celsus/file_utils.hpp>
#include <celsus/refptr.hpp>
#include <celsus/effect_wrapper.hpp>

struct FontInfo
{
  // 0, 1
  // 2, 3
  D3DXVECTOR2 _uv[4];
  int _w, _h;
  float _ofsx, _ofsy, _advance;
};

// wrapper around stb-truetype
class Font
{
public:
  Font();
  ~Font();
  bool init(const char *filename, float font_height);
  PosTex *render(const char *text, PosTex *vtx, int width, int height, const D3DXVECTOR3& ofs);
  void calc_extents(const char *text, int width, int height, int *req_width, int *req_height);
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

struct FontInstance
{
  typedef DynamicVb<PosTex> Verts;

  FontInstance(const string2& name, float h, Font *f) : font_name(name), height(h), font(f) { _verts.create(10000); }
  ~FontInstance() { delete font; }
  string2 font_name;
  float height;
  Font* font;
  Verts _verts;
};


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
  PixelRect _rc;
};

struct Node
{
  Node()
  {
    _children[0] = _children[1] = nullptr;
    _image = nullptr;
  }

  ~Node()
  {
    SAFE_DELETE(_image);
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
  , _font_height(0)
  , _texture_width(0)
  , _texture_height(0)
  , _scale(0)
{
}

Font::~Font()
{
}

bool Font::init(const char *filename, float font_height)
{
  // worst case should be square glyphs of size "font_height", and assume 256 glyps, so 16 * x * 16 * x size
  _font_height = font_height;
  _texture_width = (int)(16 * font_height);
  _texture_height = (int)(16 * font_height);
  RETURN_ON_FAIL_BOOL_E(_font_file->load(filename));
  ZeroMemory(&_font, sizeof(_font));
  if (stbtt_InitFont(&_font, _font_file->data(), stbtt_GetFontOffsetForIndex(_font_file->data(),0)) == 0)
    return false;
  _scale = stbtt_ScaleForPixelHeight(&_font, _font_height);
  pack_font();
  return true;
}

bool Font::pack_font()
{
  ID3D11Device* device = Graphics::instance().device();
  CD3D11_TEXTURE2D_DESC desc(DXGI_FORMAT_R8_UNORM, _texture_width, _texture_height, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  RETURN_ON_FAIL_BOOL_E(device->CreateTexture2D(&desc, NULL, &_texture));
  RETURN_ON_FAIL_BOOL_E(device->CreateShaderResourceView(_texture, NULL, &_view));

  auto context = Graphics::instance().context();
  D3D11_MAPPED_SUBRESOURCE s;
  context->Map(_texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &s);
  uint8_t *buf = (uint8_t *)s.pData;

  Node *root = new Node();
  root->_rc = PixelRect(0, 0, _texture_width, _texture_height);

  char char_map[256];
  for (int i = 1; i < 256; ++i) {
    char_map[i-1] = (char)i;
  }
  char_map[255] = 0;
  char *cur = char_map;

  float ww = (float)_texture_width;
  float hh = (float)_texture_height;

  while (*cur) {
    char ch = *cur;

    int advance, lsb, x0, y0, x1, y1;
    int g = stbtt_FindGlyphIndex(&_font, ch);
    stbtt_GetGlyphHMetrics(&_font, g, &advance, &lsb);
    stbtt_GetGlyphBitmapBox(&_font, g, _scale, _scale, &x0, &y0, &x1, &y1);
    int gw = x1-x0;
    int gh = y1-y0;

    Image *img = new Image();
    img->_rc = PixelRect(0, 0, gh, gw);
    if (Node *n = root->insert(img)) {
      // create texture coords

      // 0, 1
      // 2, 3
      D3DXVECTOR2 _uv[4];
      FontInfo info;
      info._uv[0] = D3DXVECTOR2(n->_rc._left / ww, n->_rc._top / hh);
      info._uv[1] = D3DXVECTOR2((n->_rc._left + img->_rc.width()) / ww, n->_rc._top / hh);
      info._uv[2] = D3DXVECTOR2(n->_rc._left / ww, (n->_rc._top + img->_rc.height()) / hh);
      info._uv[3] = D3DXVECTOR2((n->_rc._left + img->_rc.width()) / ww, (n->_rc._top + img->_rc.height()) / hh);
      info._w = img->_rc.width();
      info._h = img->_rc.height();
      info._ofsx = (float)x0;
      info._ofsy = (float)y0;
			info._advance = _scale * advance;

      _font_map.insert(std::make_pair(ch, info));
      n->_image = img;

      int x = n->_rc._left, y = n->_rc._top;
      int pw = _texture_width;
      stbtt_MakeGlyphBitmap(&_font, buf+x+y*pw, gw,gh,pw, _scale, _scale, g);
    }

/*
    if (x + gw + 1 >= pw)
      y = bottom_y, x = 1; // advance to next row
    if (y + gh + 1 >= ph) // check if it fits vertically AFTER potentially moving to next row
      return -i;
    STBTT_assert(x+gw < pw);
    STBTT_assert(y+gh < ph);
    stbtt_MakeGlyphBitmap(&f, pixels+x+y*pw, gw,gh,pw, scale,scale, g);
    chardata[i].x0 = (stbtt_int16) x;
    chardata[i].y0 = (stbtt_int16) y;
    chardata[i].x1 = (stbtt_int16) (x + gw);
    chardata[i].y1 = (stbtt_int16) (y + gh);
    chardata[i].xadvance = scale * advance;
    chardata[i].xoff     = (float) x0;
    chardata[i].yoff     = (float) y0;
*/
/*
    int w, h, ofsx, ofsy;
    uint8_t *bitmap = stbtt_GetCodepointBitmap(&_font, _scale, _scale, ch, &w, &h, &ofsx, &ofsy);
    Image *img = new Image();
    img->_rc = PixelRect(0, 0, h, w);
    if (Node *n = root->insert(img)) {
      // create texture coords

      // 0, 1
      // 2, 3
      D3DXVECTOR2 _uv[4];
      FontInfo info;
      info._uv[0] = D3DXVECTOR2(n->_rc._left / ww, n->_rc._top / hh);
      info._uv[1] = D3DXVECTOR2((n->_rc._left + img->_rc.width()) / ww, n->_rc._top / hh);
      info._uv[2] = D3DXVECTOR2(n->_rc._left / ww, (n->_rc._top + img->_rc.height()) / hh);
      info._uv[3] = D3DXVECTOR2((n->_rc._left + img->_rc.width()) / ww, (n->_rc._top + img->_rc.height()) / hh);
      info._w = img->_rc.width();
      info._h = img->_rc.height();
      info._ofsx = ofsx;
      info._ofsy = ofsy;

      _font_map.insert(std::make_pair(ch, info));

      n->_image = img;
      int x = n->_rc._left, y = n->_rc._top;
      for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
          // assume ptr contains 32 bit data
          int c = bitmap[j*w+i];
          int ofs = 4 * ((y+j) * _texture_width + x + i);
          buf[ofs+0] = buf[ofs+1] = buf[ofs+2] = buf[ofs+3] = c; 
        }
      }

    } else {
      SAFE_DELETE(img);
    }
    stbtt_FreeBitmap(bitmap, NULL);
*/
    ++cur;
  }

  //save_bmp32("c:/temp/tjong.bmp", buf, _texture_width, _texture_height);

  context->Unmap(_texture, 0);
  return true;
}

void Font::calc_extents(const char *text, int width, int height, int *req_width, int *req_height)
{
  D3DXVECTOR3 pos(0, 0, 0);
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

    FontMap::const_iterator it = _font_map.find(ch);
    if (it == _font_map.end()) {
      ++text;
      continue;
    }

    const FontInfo& info = it->second;

    if (pos.x + info._advance > width)
      new_lines = 1;
    if (new_lines) {
      // check if it's possible..
      pos.y += new_lines * (max_height != 0 ? max_height : (int)_font_height);
      if (pos.y > height)
        break;
      pos.x  = 0;
      max_height = 0;
    }

    pos.x += info._advance;
    max_height = std::max<int>(max_height, (int)_font_height);
    ++text;
  }
  pos.y += _font_height;

  *req_width = (int)pos.x;
  *req_height = (int)pos.y;
}

PosTex *Font::render(const char *text, PosTex *vtx, int width, int height, const D3DXVECTOR3& ofs)
{
  if (!text)
    return vtx;

  const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

  D3DXVECTOR3 pos(ofs);
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

    FontMap::const_iterator it = _font_map.find(ch);
    if (it == _font_map.end()) {
      ++text;
      continue;
    }

    const FontInfo& info = it->second;
    // 0, 1
    // 2, 3

    auto v0 = PosTex(screen_to_clip(pos + D3DXVECTOR3((float)info._ofsx + 0, info._ofsy + _font_height + 0,0), viewport), info._uv[0]);
    auto v1 = PosTex(screen_to_clip(pos + D3DXVECTOR3((float)info._ofsx + info._w, info._ofsy + _font_height + 0,0), viewport), info._uv[1]);
    auto v2 = PosTex(screen_to_clip(pos + D3DXVECTOR3((float)info._ofsx + 0.0f, info._ofsy + _font_height + info._h,0), viewport), info._uv[2]);
    auto v3 = PosTex(screen_to_clip(pos + D3DXVECTOR3((float)info._ofsx + info._w, info._ofsy + _font_height + info._h,0), viewport), info._uv[3]);

    // 2, 0, 1
    // 2, 1, 3
    *vtx++ = v2;
    *vtx++ = v0;
    *vtx++ = v1;

    *vtx++ = v2;
    *vtx++ = v1;
    *vtx++ = v3;

    if (pos.x + info._advance > width)
      new_lines = 1;
    if (new_lines) {
      // check if it's possible..
      pos.y += new_lines * (max_height != 0 ? max_height : (int)_font_height);
      if (pos.y > height)
        break;
      pos.x  = 0;
      max_height = 0;
    }

    pos.x += info._advance;
    max_height = std::max<int>(max_height, (int)_font_height);
    ++text;
  }
  return vtx;
}


FontWriter::FontWriter()
  : _top(-1)
  , _left(-1)
  , _width(-1)
  , _height(-1)
{
}

FontWriter::~FontWriter()
{
  close();
  container_delete(_fonts);
}

bool FontWriter::init(const string2& font_name, int top, int left, int width, int height)
{
  _font_name = font_name;
  _top = top; 
  _left = left;
  _width = width; 
  _height = height;

	using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_writer2.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
		MakeDelegate(this, &FontWriter::load_effect)));

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/debug_writer.lua", System::kDirRelative), MakeDelegate(this, &FontWriter::load_states), true));

	return true;
}

FontInstance *FontWriter::find_font(float height)
{
  // finds an instance of the font with the given size, or creates a new one
  for (auto i = _fonts.begin(), e = _fonts.end(); i != e; ++i) {
    if ((*i)->height == height)
      return (*i);
  }

  // create new font
  Font *f = new Font();
  if (!f->init(_font_name, height))
    return NULL;
  _fonts.push_back(new FontInstance(_font_name, height, f));
  return _fonts.back();
}

void FontWriter::render()
{
	auto context = Graphics::instance().context();

  // setup states shared between all fonts
  _effect->set_shaders(context);
  ID3D11SamplerState *samplers[] = { _sampler_state };
  context->PSSetSamplers(0, 1, samplers);

  float blend_factor[] = { 1, 1, 1, 1 };
  context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);
  context->OMSetDepthStencilState(_dss, 0xffffffff);

  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto i = _text_segments.begin(); i != _text_segments.end(); ++i) {

    Font *f = i->first->font;
    FontInstance::Verts *verts = &i->first->_verts;
    PosTex *v = verts->map();
    int chars = 0;
    const std::vector<TextSegment>& s = i->second;
    for (auto j = s.begin(), je = s.end(); j != je; ++j) {
      v = f->render(j->text, v, _width, _height, j->pos);
      chars += j->text.size();
    }

    verts->unmap();

    // draw current font
    ID3D11ShaderResourceView* t[] = { f->view()};
    context->PSSetShaderResources(0, 1, t);

    set_vb(context, verts->get(), FontInstance::Verts::stride);
    context->Draw(6 * chars, 0);
  }
}

void FontWriter::reset_frame()
{
	_text_segments.clear();
}

void FontWriter::calc_extents(int *width, int *height, float h, const char *fmt, ...)
{
	FontInstance *f = find_font(h);
	if (!f)
		return;

	va_list arg;
	va_start(arg, fmt);

	const int len = _vscprintf(fmt, arg) + 1;
	char* buf = (char*)_alloca(len);
	vsprintf_s(buf, len, fmt, arg);

	f->font->calc_extents(buf, _width, _height, width, height);
}

void FontWriter::write(const int left, const int top, float h, const char *fmt, ...)
{
  FontInstance *f = find_font(h);
  if (!f)
    return;

  va_list arg;
  va_start(arg, fmt);

  const int len = _vscprintf(fmt, arg) + 1;
  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, fmt, arg);

  _text_segments[f].push_back(TextSegment(D3DXVECTOR3((float)left, (float)top, 0), string2(buf), h));
  va_end(arg);
}

void FontWriter::close()
{

}

void FontWriter::load_effect(EffectWrapper *effect)
{
	_effect.reset(effect);
	InputDesc().
		add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0).
		create(_layout, _effect.get());
}

bool FontWriter::load_states(const string2& filename)
{
	auto& s = System::instance();
	if (!lua_load_states(filename, "default_blend", "default_dss", "default_sampler", &_blend_state.p, &_dss.p, &_sampler_state.p))
		return false;

	return true;
}
