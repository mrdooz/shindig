#pragma once
#include "dynamic_vb.hpp"
#include <celsus/vertex_types.hpp>

class DebugMenu
{
public:

	typedef std::function<void()> ButtonCallback;

	static DebugMenu& instance();
	bool init();
	bool close();

	LRESULT wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void render();
	void add_item(const char *text);
	void add_button(const char *text, const ButtonCallback& cb);

private:
	DebugMenu();
	~DebugMenu();

	struct Settings
	{
    int x, y;
		int w, h;
    int border;
    int spacing;
    uint32_t col_fg, col_bg;
    uint32_t col_mouse_over, col_clicked;
	};

  enum ButtonState {
    kStateDefault,
    kStateMouseOver,
    kStateClicked,
  };

  struct MenuButton
  {
    MenuButton(const string2& text, const ButtonCallback& cb) : text(text), cb(cb), state(kStateDefault) {}
    string2 text;
    ButtonCallback cb;
    ButtonState state;
    D3DXVECTOR3 center;
    D3DXVECTOR3 extents;
  };

  struct MenuItem
  {
    string2 text;
  };

	static DebugMenu *_instance;
	bool load_settings(const string2& filename);
	void load_effect(EffectWrapper *effect);
  void create_menu();
  DWORD color_from_state(ButtonState state);
  MenuButton *point_in_button(const POINTS& pt);
  void reset_button_states();

  typedef DynamicVb<PosCol> Verts;
	Verts _vb;
	CComPtr<ID3D11InputLayout> _layout;
	CComPtr<ID3D11BlendState> _blendstate;
	CComPtr<ID3D11DepthStencilState> _dss;

  std::vector<MenuButton> _buttons;

	Settings _settings;

	EffectWrapper *_effect;
};
