#include "stdafx.h"
#include "debug_menu.hpp"
#include "system.hpp"
#include "resource_manager.hpp"
#include <celsus/text_scanner.hpp>

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

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("shindig/debug_menu.cfg", System::kDirRelative), 
		MakeDelegate(this, &DebugMenu::load_settings), true));

	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_menu.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
		MakeDelegate(this, &DebugMenu::load_effect)));

	if (!_vb.create(10000))
		return false;

	_dss.Attach(rt::D3D11::DepthStencilDescription().Create(d));
	_blendstate.Attach(rt::D3D11::BlendDescription().Create(d));

	return true;
}

bool DebugMenu::close()
{
	SAFE_DELETE(_effect);
	return true;
}

LRESULT DebugMenu::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return 0; 
}

bool DebugMenu::load_settings(const string2& filename)
{
	TextScanner scanner;
	if (!scanner.load(filename))
		return false;

	const char *buf;
	int len;
	if (!scanner.read_line(&buf, &len))
		return false;

	return true;
}

void DebugMenu::load_effect(EffectWrapper *effect)
{
	SAFE_DELETE(effect);
	_effect = effect;

	InputDesc().
		add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
		create(_layout, _effect);
}

DebugMenu& DebugMenu::instance()
{
	if (_instance == NULL)
		_instance = new DebugMenu();
	return *_instance;
}

void DebugMenu::add_item(const char *text)
{

}

void DebugMenu::add_button(const char *text, const ButtonCallback& cb)
{

}

void DebugMenu::render()
{

}
