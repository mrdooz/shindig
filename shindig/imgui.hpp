#pragma once

#include "dynamic_vb.hpp"
#include <celsus/vertex_types.hpp>

#ifdef IMGUI_SRC_ID
#define GEN_ID ((IMGUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif

// IMGUI state
struct UIState
{
  UIState() : mouse_x(0), mouse_y(0), mouse_down(0), hot_item(0), active_item(0), kbd_item(0), key_entered(0), key_mod(0), last_widget(0) {}
  int mouse_x;
  int mouse_y;
  int mouse_down;

  int hot_item;
  int active_item;

  int kbd_item;
  int key_entered;
  int key_mod;
  int last_widget;
};


class IMGui
{
public:
  static IMGui& instance();
  UIState& ui_state() { return _ui_state; }

  bool init();
  bool close();

  bool init_frame();
  bool render();

	// in screen space
	void add_rect(int x, int y, int width, int height, const D3DXCOLOR& color);

	static int button(int id, int x, int y, int width, int height);

private:
  IMGui();

  void load_effect(EffectWrapper *effect);
  bool load_states(const string2& filename);

  UIState _ui_state;

  typedef DynamicVb<PosCol> Verts;
  Verts _verts;

  CComPtr<ID3D11InputLayout> _layout;
  CComPtr<ID3D11DepthStencilState> _dss;
  CComPtr<ID3D11BlendState> _blend_state;
	std::auto_ptr<EffectWrapper> _effect;
	PosCol *_vtx;

  static IMGui *_instance;
};