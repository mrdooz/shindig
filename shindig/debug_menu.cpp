#include "stdafx.h"
#include "debug_menu.hpp"
#include "system.hpp"
#include "resource_manager.hpp"
#include <celsus/text_scanner.hpp>

DebugMenu *DebugMenu::_instance = nullptr;

D3DXVECTOR3 vp_to_screen(const D3D11_VIEWPORT& vp, const D3DXVECTOR3& pt)
{
  return D3DXVECTOR3(pt.x + vp.Width / 2, vp.Height/2 - pt.y, pt.z);
}

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

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("shindig/debug_menu.cfg", System::kDirRelative), 
		MakeDelegate(this, &DebugMenu::load_settings), true));

	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_menu.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
		MakeDelegate(this, &DebugMenu::load_effect)));


  using namespace rt;
  _dss.Attach(D3D11::DepthStencilDescription().DepthEnable_(FALSE).Create(d));
	_blendstate.Attach(D3D11::BlendDescription().Create(d));

	const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();
	_writer.init((int)viewport.Width, (int)viewport.Height);

	return true;
}

bool DebugMenu::close()
{
	SAFE_DELETE(_effect);
	return true;
}

DebugMenu::MenuButton *DebugMenu::point_in_button(const POINTS& pt)
{
  const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

  for (int i = 0; i < (int)_buttons.size(); ++i) {
    MenuButton& cur = _buttons[i];
    D3DXVECTOR3 center(vp_to_screen(viewport, cur.center));
    ButtonState old_state = cur.state;
    ButtonState new_state = cur.state;
    if (pt.x >= center.x - cur.extents.x && pt.x < center.x + cur.extents.x && 
      pt.y >= center.y - cur.extents.y && pt.y < center.y + cur.extents.y)
      return &cur;
  }
  return NULL;
}

bool DebugMenu::reset_button_states()
{
	bool state_changed = false;
  for (int i = 0; i < (int)_buttons.size(); ++i) {
		state_changed |= _buttons[i].state != kStateDefault;
		_buttons[i].state = kStateDefault;
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
      if (MenuButton *btn = point_in_button(MAKEPOINTS(lParam))) {
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
			if (MenuButton *btn = point_in_button(MAKEPOINTS(lParam))) {
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
      if (MenuButton *btn = point_in_button(MAKEPOINTS(lParam))) {
        btn->cb();
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
	TextScanner scanner;
  scanner.set_line_mode(true);
	if (!scanner.load(filename))
		return false;

  // read window pos and size
  std::vector<int> dims;
  if (!scanner.read_ints(&dims))
    return false;
  _settings.x = dims[0];
  _settings.y = dims[1];

  // read window size
  if (!scanner.read_ints(&dims))
    return false;
  _settings.w = dims[0];
  _settings.h = dims[1];

  if (!scanner.read_int(&_settings.border))
    return false;

  if (!scanner.read_int(&_settings.spacing))
    return false;

  // read colors
  if (!scanner.read_hex(&_settings.col_bg))
    return false;

  if (!scanner.read_hex(&_settings.col_fg))
    return false;

  if (!scanner.read_hex(&_settings.col_mouse_over))
    return false;

  if (!scanner.read_hex(&_settings.col_clicked))
    return false;

  create_menu();

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

void DebugMenu::add_label()
{

}

void DebugMenu::add_item(const char *text)
{

}


void DebugMenu::add_button(const char *text, const ButtonCallback& cb)
{
  MenuButton btn(text, cb);

  // calc pos

  const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

  const float w = viewport.Width;
  const float h = viewport.Height;
  const float extents_x = 0.5f * _settings.w;
  const float extents_y = 0.5f * _settings.h;
  D3DXVECTOR3 pos(-w / 2 + _settings.x + extents_x, +h / 2 - _settings.y - extents_y, 0);
  pos.y -= _buttons.size() * (_settings.h + _settings.spacing);
  btn.center = pos;
  btn.extents = D3DXVECTOR3(extents_x, extents_y, 0);

  _buttons.push_back(btn);

  create_menu();
}

void DebugMenu::render()
{
  auto context = Graphics::instance().context();
  _effect->set_shaders(context);
  set_vb(context, _vb.vb(), Verts::stride);
  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->OMSetDepthStencilState(_dss, 0xffffffff);
  context->Draw(2 * 6 * _buttons.size(), 0);

	_writer.reset_frame();
	for (int i = 0; i < (int)_buttons.size(); ++i) {
		const MenuButton& cur = _buttons[i];
		_writer.write(cur.center, cur.text);
	}
	_writer.render();
}


DWORD DebugMenu::color_from_state(ButtonState state)
{
  switch (state)
  {
  case kStateMouseOver:
    return _settings.col_mouse_over;
  case kStateClicked:
    return _settings.col_clicked;
  default:
    LOG_ERROR_LN("Unknown state");
  case kStateDefault:
    return _settings.col_fg;
  }
}

D3DXVECTOR3 scale_to_clipspace(const D3DXVECTOR3& v, float width, float height)
{
  return D3DXVECTOR3(2 * v.x / width, 2 * v.y / height, v.z);
}

void DebugMenu::create_menu()
{
  PosCol *vtx = _vb.map();

  // map to clipspace, -1..1
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

  D3DXVECTOR3 v0(-extents_x, +extents_y, 0);
  D3DXVECTOR3 v1(+extents_x, +extents_y, 0);
  D3DXVECTOR3 v2(-extents_x, -extents_y, 0);
  D3DXVECTOR3 v3(+extents_x, -extents_y, 0);

  D3DXVECTOR3 b0(-extents_x + _settings.border, +extents_y - _settings.border, 0);
  D3DXVECTOR3 b1(+extents_x - _settings.border, +extents_y - _settings.border, 0);
  D3DXVECTOR3 b2(-extents_x + _settings.border, -extents_y + _settings.border, 0);
  D3DXVECTOR3 b3(+extents_x - _settings.border, -extents_y + _settings.border, 0);

  for (int i = 0; i < (int)_buttons.size(); ++i) {
    const MenuButton& btn = _buttons[i];
    const D3DXVECTOR3& pos = btn.center;

    // draw outer
    vtx->pos = scale_to_clipspace(pos + v0, w, h); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + v1, w, h); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + v2, w, h); vtx->col = _settings.col_bg; ++vtx;

    vtx->pos = scale_to_clipspace(pos + v2, w, h); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + v1, w, h); vtx->col = _settings.col_bg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + v3, w, h); vtx->col = _settings.col_bg; ++vtx;

    // draw inner
		D3DXCOLOR fg =  color_from_state(btn.state); 
    vtx->pos = scale_to_clipspace(pos + b0, w, h); vtx->col = fg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + b1, w, h); vtx->col = fg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + b2, w, h); vtx->col = fg; ++vtx;

    vtx->pos = scale_to_clipspace(pos + b2, w, h); vtx->col = fg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + b1, w, h); vtx->col = fg; ++vtx;
    vtx->pos = scale_to_clipspace(pos + b3, w, h); vtx->col = fg; ++vtx;

    //pos.y -= (_settings.spacing + extents_y * 2);
  }

  _vb.unmap();
}
