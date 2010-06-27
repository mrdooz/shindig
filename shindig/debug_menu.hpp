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
		int w, h;
		D3DXCOLOR bg_color, fg_color;
	};

	static DebugMenu *_instance;
	bool load_settings(const string2& filename);
	void load_effect(EffectWrapper *effect);

	DynamicVb<PosCol> _vb;
	CComPtr<ID3D11InputLayout> _layout;
	CComPtr<ID3D11BlendState> _blendstate;
	CComPtr<ID3D11DepthStencilState> _dss;

	Settings _settings;

	EffectWrapper *_effect;
};