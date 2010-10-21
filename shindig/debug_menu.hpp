#pragma once
#include "font_writer.hpp"

class DebugMenu
{
public:

	enum AlignFlags {
		kAlignHoriz = 0,
		kAlignVert = 1 << 8,
		kAlignLeft = 1,
		kAlignRight = 2,
		kAlignCenter = 3,
	};

	typedef std::function<void()> ButtonCallback;

	static DebugMenu& instance();
	bool init();
	bool close();

	LRESULT wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	void render();
	void add_item(const char *text);
	void add_label(const char *text, int *ptr);
	void add_label(const char *text, float *ptr);
	void add_label(const char *text, bool *ptr);
	void add_label(const char *text, D3DXVECTOR3 *ptr);
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

	struct ButtonBase
	{
		ButtonBase(const string2& text) : text(text), state(kStateDefault) {}
		virtual ~ButtonBase() {}
		virtual bool apply() { return true; }
		virtual bool update() { return true; }
		virtual bool clickable() const { return false; }
		bool point_inside(const POINTS& pt) const;
		D3DXVECTOR3 center;
		D3DXVECTOR3 extents;
		string2 text;
		ButtonState state;
	};

  struct MenuButton : public ButtonBase
  {
    MenuButton(const string2& text, const ButtonCallback& cb) : ButtonBase(text), cb(cb) {}
		virtual bool apply() { cb(); return true; }
		virtual bool clickable() const { return true; }
    ButtonCallback cb;
  };

  struct MenuItem
  {
    string2 text;
  };

	struct MenuLabel : public ButtonBase
	{
		enum Type {
			kTypeInt,
			kTypeFloat,
			kTypeBool,
			kTypeVector3
		};

		union Value {
			int *_int;
			float *_float;
			bool *_bool;
			D3DXVECTOR3 *_v3;
		}; 

		MenuLabel(const string2& text, int *ptr) : ButtonBase(""), _prefix(text), _type(kTypeInt) { _value._int = ptr; }
		MenuLabel(const string2& text, D3DXVECTOR3 *ptr) : ButtonBase(""), _prefix(text), _type(kTypeVector3) { _value._v3 = ptr; }

		virtual bool update();

		string2 _prefix;
		Value _value;
		Type _type;
	};

	static DebugMenu *_instance;
	bool load_settings(const string2& filename);
	void load_effect(EffectWrapper *effect);
  void create_menu();
  DWORD color_from_state(const ButtonBase *btn) const;
  ButtonBase *point_in_button(const POINTS& pt);
  bool reset_button_states();

  typedef DynamicVb<PosCol> Verts;
	Verts _vb;
	CComPtr<ID3D11InputLayout> _layout;
	CComPtr<ID3D11BlendState> _blendstate;
	CComPtr<ID3D11DepthStencilState> _dss;

  std::vector<ButtonBase *> _buttons;

	FontWriter _writer;
	Settings _settings;

	EffectWrapper *_effect;
};
