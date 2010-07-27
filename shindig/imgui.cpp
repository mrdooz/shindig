#include "stdafx.h"
#include "imgui.hpp"
#include "system.hpp"
#include <celsus/graphics.hpp>
#include "resource_manager.hpp"
#include "lua_utils.hpp"

IMGui *IMGui::_instance = nullptr;

namespace
{
	bool inside_region(int x, int y, int width, int height)
	{
		const UIState& u = IMGui::instance().ui_state();
		return u.mouse_x >= x && u.mouse_x < x + width && u.mouse_y > y && u.mouse_y < y + height;
	}
}

IMGui& IMGui::instance()
{
  if (!_instance)        
    _instance = new IMGui();
  return *_instance;
}

IMGui::IMGui()
	: _effect(nullptr)
{

}

bool IMGui::init()
{
  using namespace fastdelegate;
  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  Graphics& g = Graphics::instance();
  ID3D11Device *d = g.device();

	RETURN_ON_FAIL_BOOL_E(_verts.create(10000));
  RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/imgui_states.lua", System::kDirRelative), MakeDelegate(this, &IMGui::load_states), true));
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/imgui.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &IMGui::load_effect)));

	RETURN_ON_FAIL_BOOL_E(_writer.init(System::instance().convert_path("data/fonts/TCB_____.ttf", System::kDirRelative), 0, 0, 1000, 1000));

  return true;
}

bool IMGui::close()
{
	_writer.close();
  return true;
}

bool IMGui::init_frame()
{
	_writer.reset_frame();
	_ui_state.hot_item = 0;
	_vtx = _verts.map();
  return true;
}

bool IMGui::render()
{
	int num_verts = _verts.unmap(_vtx);

	if (_ui_state.mouse_down == 0) {
		_ui_state.active_item = 0;
	} else {
		if (_ui_state.active_item == 0)
			_ui_state.active_item = -1;
	}

	ID3D11Device* device = Graphics::instance().device();
	ID3D11DeviceContext* context = Graphics::instance().context();

	float blend_factors[] = {1, 1, 1, 1};
	context->OMSetDepthStencilState(_dss, 0);

	float blend_factor[] = { 1, 1, 1, 1 };
	context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);

	_effect->set_shaders(context);

	context->IASetInputLayout(_layout);
	set_vb(context, _verts.vb(), Verts::stride);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(num_verts, 0);

	_writer.render();

  return true;
}

void IMGui::add_rect(int x, int y, int width, int height, const D3DXCOLOR& color, const char *text)
{

	const D3D11_VIEWPORT& viewport = Graphics::instance().viewport();

	// 0--1
	// |  |
	// 2--3
	// 0, 1, 2
	// 2, 1, 3

	float w = (float)width;
	float h = (float)height;
	PosCol v0(screen_to_clip(D3DXVECTOR3((float)x    , (float)y    , 0), viewport), color);
	PosCol v1(screen_to_clip(D3DXVECTOR3((float)x + w, (float)y    , 0), viewport), color);
	PosCol v2(screen_to_clip(D3DXVECTOR3((float)x    , (float)y + h, 0), viewport), color);
	PosCol v3(screen_to_clip(D3DXVECTOR3((float)x + w, (float)y + h, 0), viewport), color);

	*_vtx++ = v0;
	*_vtx++ = v1;
	*_vtx++ = v2;

	*_vtx++ = v2;
	*_vtx++ = v1;
	*_vtx++ = v3;

	if (text) {
		int w, h;
		_writer.calc_extents(&w, &h, 16, text);
		_writer.write(x + (width - w) / 2, y + (height-h)/2, 16, text);
	}
}

bool IMGui::load_states(const string2& filename)
{
  auto& s = System::instance();
  if (!lua_load_states(filename, "default_blend", "default_dss", NULL, &_blend_state.p, &_dss.p, NULL))
    return false;

  return true;
}

void IMGui::load_effect(EffectWrapper *effect)
{
	_effect.reset(effect);
  InputDesc(). 
    add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
    add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
    create(_layout, _effect.get());
}

int IMGui::button(int id, int x, int y, int width, int height, const char *text)
{
	UIState& u = IMGui::instance().ui_state();

	if (inside_region(x, y, width, height)) {
		u.hot_item = id;
		if (u.active_item == 0 && u.mouse_down)
			u.active_item = id;
	}
	
	IMGui::instance().add_rect(x, y, width, height, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f), text);

	const bool is_hot = u.hot_item == id;
	const bool is_active = u.active_item == id;
	if (is_hot) {
		if (is_active) {
			// hot && active
			IMGui::instance().add_rect(x, y, width, height, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), text);
		} else {
			// hot
			IMGui::instance().add_rect(x, y, width, height, D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f), text);
		}
	}

	// if button is hot & active, and mouse is up, then the button has been clicked
	return is_hot && is_active && !u.mouse_down;
}
