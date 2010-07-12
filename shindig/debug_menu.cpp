#include "stdafx.h"
#include "debug_menu.hpp"
#include "system.hpp"
#include "resource_manager.hpp"
#include <celsus/text_scanner.hpp>
#include "lua_utils.hpp"

// Note, all the debug code stuff uses pixels as units, with a coordinate system
// like so: 
//
// (0,0) ---------> x+
//   |
//   |
//  +y
//

DebugMenu *DebugMenu::_instance = nullptr;

DebugMenu::DebugMenu()
	: _effect(nullptr)
{
}

DebugMenu::~DebugMenu()
{
	close();
}

bool DebugMenu::init()
{
	using namespace fastdelegate;
	auto& s = System::instance();
	auto& r = ResourceManager::instance();
	Graphics& g = Graphics::instance();
	ID3D11Device *d = g.device();

  if (!_vb.create(10000))
    return false;

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("shindig/debug_menu.lua", System::kDirRelative), 
		MakeDelegate(this, &DebugMenu::load_settings), true));

	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_menu.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
		MakeDelegate(this, &DebugMenu::load_effect)));


  using namespace rt;
  _dss.Attach(D3D11::DepthStencilDescription().DepthEnable_(FALSE).Create(d));
	_blendstate.Attach(D3D11::BlendDescription().Create(d));

	const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

  RETURN_ON_FAIL_BOOL_E(_writer.init(s.convert_path("data/fonts/arial.ttf", System::kDirRelative), 0, 0, 600, 600));

	return true;
}

bool DebugMenu::close()
{
	SAFE_DELETE(_effect);
	return true;
}

bool DebugMenu::ButtonBase::point_inside(const POINTS& pt) const
{
	return pt.x >= center.x - extents.x && pt.x < center.x + extents.x && 
		pt.y >= center.y - extents.y && pt.y < center.y + extents.y;
}


DebugMenu::ButtonBase *DebugMenu::point_in_button(const POINTS& pt)
{
  for (int i = 0; i < (int)_buttons.size(); ++i) {
    ButtonBase *cur = _buttons[i];
		if (cur->point_inside(pt))
			return cur;
  }
  return NULL;
}

bool DebugMenu::reset_button_states()
{
	bool state_changed = false;
  for (int i = 0; i < (int)_buttons.size(); ++i) {
		state_changed |= _buttons[i]->state != kStateDefault;
		_buttons[i]->state = kStateDefault;
	}
	return state_changed;
}

LRESULT DebugMenu::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_MOUSEMOVE:
    {
			bool changed = reset_button_states();
      if (ButtonBase *btn = point_in_button(MAKEPOINTS(lParam))) {
				changed |= btn->state != kStateMouseOver;
				btn->state = kStateMouseOver;
      }
			if (changed)
				create_menu();
			return TRUE;
    }

	case WM_LBUTTONDOWN:
		{
			bool changed = reset_button_states();
			if (ButtonBase *btn = point_in_button(MAKEPOINTS(lParam))) {
				btn->state = kStateClicked;
				changed = true;
			}
			if (changed)
				create_menu();
			return TRUE;
		}

  case WM_LBUTTONUP:
    {
			bool changed = reset_button_states();
      if (ButtonBase *btn = point_in_button(MAKEPOINTS(lParam))) {
        btn->apply();
				btn->state = kStateMouseOver;
				changed = true;
      }
			if (changed)
				create_menu();
			return TRUE;

    }
    break;
  }
	return 0; 
}

bool DebugMenu::load_settings(const string2& filename)
{
	lua_State *l;
	if (!lua_init(&l, filename))
		return false;

	SCOPED_OBJ([l](){lua_close(l); } );

	if (lua_pcall(l, 0, 0, 0)) {
		LOG_WARNING_LN(lua_tostring(l, -1));
		return false;
	}

	// push state table on stack
	lua_getglobal(l, "debug_menu");

	try {
		Settings tmp;
		tmp.x = get_int_field(l, "left");
		tmp.y = get_int_field(l, "top");

		tmp.w = get_int_field(l, "item_size_w");
		tmp.h = get_int_field(l, "item_size_h");

		tmp.border = get_int_field(l, "border");
		tmp.spacing = get_int_field(l, "spacing");

		tmp.col_bg = (uint32_t)get_int_field(l, "background");
		tmp.col_fg = (uint32_t)get_int_field(l, "foreground");
		tmp.col_mouse_over = (uint32_t)get_int_field(l, "mouse-over");
		tmp.col_clicked = (uint32_t)get_int_field(l, "clicked");
		_settings = tmp;
		create_menu();
	} catch (std::runtime_error&) {
		return false;
	}
	return true;
}

void DebugMenu::load_effect(EffectWrapper *effect)
{
	SAFE_DELETE(_effect);
	_effect = effect;

	InputDesc().
		add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
		create(_layout, _effect);
}

DebugMenu& DebugMenu::instance()
{
	if (_instance == NULL)
		_instance = new DebugMenu();
	return *_instance;
}

void DebugMenu::render()
{
	for (int i = 0; i < (int)_buttons.size(); ++i)
		_buttons[i]->update();

	auto context = Graphics::instance().context();
	_effect->set_shaders(context);
	set_vb(context, _vb.vb(), Verts::stride);
	context->IASetInputLayout(_layout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->OMSetDepthStencilState(_dss, 0xffffffff);
	float blend_factor[] = { 1, 1, 1, 1 };
	context->OMSetBlendState(_blendstate, blend_factor, 0xffffffff);
	context->Draw(2 * 6 * _buttons.size(), 0);

	_writer.reset_frame();
	const float height = 20;
	for (int i = 0; i < (int)_buttons.size(); ++i) {
		const ButtonBase *cur = _buttons[i];
		int w, h;
		_writer.calc_extents(&w, &h, height, cur->text);
		_writer.write((int)(cur->center.x - w / 2), (int)(cur->center.y - h / 2), height, cur->text);
	}
	_writer.render();

}

void DebugMenu::add_button(const char *text, const ButtonCallback& cb)
{
  MenuButton *btn = new MenuButton(text, cb);
  const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

	// calc center and extents
  const float extents_x = 0.5f * _settings.w;
  const float extents_y = 0.5f * _settings.h;
  btn->center = D3DXVECTOR3(
    (float)_settings.x + extents_x, 
    _settings.y + extents_y + (float)_buttons.size() * (_settings.h + _settings.spacing), 0);
  btn->extents = D3DXVECTOR3(extents_x, extents_y, 0);

  _buttons.push_back(btn);
  create_menu();
}

void DebugMenu::add_label(const char *text, int *ptr)
{

}

void DebugMenu::add_label(const char *text, float *ptr)
{

}

void DebugMenu::add_label(const char *text, bool *ptr)
{

}

void DebugMenu::add_label(const char *text, D3DXVECTOR3 *ptr)
{
	MenuLabel *btn = new MenuLabel(text, ptr);
	const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

	// calc center and extents
	const float extents_x = 0.5f * _settings.w;
	const float extents_y = 0.5f * _settings.h;
	btn->center = D3DXVECTOR3(
		(float)_settings.x + extents_x, 
		_settings.y + extents_y + (float)_buttons.size() * (_settings.h + _settings.spacing), 0);
	btn->extents = D3DXVECTOR3(extents_x, extents_y, 0);

	_buttons.push_back(btn);
	create_menu();

}

DWORD DebugMenu::color_from_state(const ButtonBase *btn) const
{
  switch (btn->state)
  {
  case kStateMouseOver:
    return _settings.col_mouse_over;
  case kStateClicked:
    return btn->clickable() ? _settings.col_clicked : _settings.col_mouse_over;
  default:
    LOG_ERROR_LN("Unknown state");
  case kStateDefault:
    return _settings.col_fg;
  }
}

void DebugMenu::create_menu()
{
  PosCol *vtx = _vb.map();

  // 0--1
  // |  |
  // 2--3
  // 0, 1, 2
  // 2, 1, 3


  const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

  const float w = viewport.Width;
  const float h = viewport.Height;
  const float extents_x = 0.5f * _settings.w;
  const float extents_y = 0.5f * _settings.h;

  D3DXVECTOR3 v0(-extents_x, -extents_y, 0);
  D3DXVECTOR3 v1(+extents_x, -extents_y, 0);
  D3DXVECTOR3 v2(-extents_x, +extents_y, 0);
  D3DXVECTOR3 v3(+extents_x, +extents_y, 0);

  D3DXVECTOR3 b0(-extents_x + _settings.border, -extents_y + _settings.border, 0);
  D3DXVECTOR3 b1(+extents_x - _settings.border, -extents_y + _settings.border, 0);
  D3DXVECTOR3 b2(-extents_x + _settings.border, +extents_y - _settings.border, 0);
  D3DXVECTOR3 b3(+extents_x - _settings.border, +extents_y - _settings.border, 0);

  for (int i = 0; i < (int)_buttons.size(); ++i) {
    const ButtonBase *btn = _buttons[i];
    const D3DXVECTOR3& pos = btn->center;

    // draw outer
    vtx->pos = screen_to_clip(pos + v0, viewport); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = screen_to_clip(pos + v1, viewport); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = screen_to_clip(pos + v2, viewport); vtx->col = _settings.col_bg; ++vtx;

    vtx->pos = screen_to_clip(pos + v2, viewport); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = screen_to_clip(pos + v1, viewport); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = screen_to_clip(pos + v3, viewport); vtx->col = _settings.col_bg; ++vtx;

    // draw inner
		D3DXCOLOR fg =  color_from_state(btn); 
    vtx->pos = screen_to_clip(pos + b0, viewport); vtx->col = fg; ++vtx;
    vtx->pos = screen_to_clip(pos + b1, viewport); vtx->col = fg; ++vtx;
    vtx->pos = screen_to_clip(pos + b2, viewport); vtx->col = fg; ++vtx;

    vtx->pos = screen_to_clip(pos + b2, viewport); vtx->col = fg; ++vtx;
    vtx->pos = screen_to_clip(pos + b1, viewport); vtx->col = fg; ++vtx;
    vtx->pos = screen_to_clip(pos + b3, viewport); vtx->col = fg; ++vtx;
  }

  _vb.unmap();
}

bool DebugMenu::MenuLabel::update()
{
	switch (_type) {
	case kTypeVector3:
		text = string2::fmt("%s: %f, %f, %f", _prefix.c_str(), _value._v3->x, _value._v3->y, _value._v3->z);
		break;
	}
	return true;
}
