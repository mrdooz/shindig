#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"
#include "test_effect.hpp"
#include "test_effect2.hpp"
#include "test_effect3.hpp"
#include "test_effect4.hpp"
#include "test_effect5.hpp"
#include "test_effect6.hpp"
#include "test_effect7.hpp"
#include "imgui.hpp"
#include "font_writer.hpp"
#include "debug_menu.hpp"
#include "debug_renderer.hpp"
#include <celsus/Profiler.hpp>
#include "iprof/prof.h"
#include "iprof/prof_internal.h"
#include "network.hpp"


App* App::_instance = nullptr;

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif



App::App()
	: _hinstance(NULL)
	, _width(-1)
	, _height(-1)
	, _hwnd(NULL)
	, _test_effect(NULL)
	, _debug_writer(nullptr)
	, _state_wireframe("wireframe", false)
	, _dbg_message_count(0)
	, _trackball(nullptr)
	, _freefly(nullptr)
	, _cur_camera(1)
	, _draw_plane(false)
{
	find_app_root();
}

App::~App()
{
}

App& App::instance()
{
	if (_instance == NULL)
		_instance = new App();
	return *_instance;
}

bool App::init(HINSTANCE hinstance)
{
	_hinstance = hinstance;
	_width = GetSystemMetrics(SM_CXSCREEN);
	_height = GetSystemMetrics(SM_CYSCREEN);
	create_window();

	Graphics& graphics = Graphics::instance();

	RETURN_ON_FAIL_BOOL_E(System::instance().init());
	RETURN_ON_FAIL_BOOL_E(graphics.init_directx(_hwnd, _width, _height));

	TwInit(TW_DIRECT3D11, graphics.device(), graphics.context());
	TwWindowSize(_width, _height);
	_tweakbar = TwNewBar("Shindig");
	TwAddButton(_tweakbar, "quit", &App::tramp_menu, (void *)kMenuQuit, "label='quit'");
	TwAddButton(_tweakbar, "debug", &App::tramp_menu, (void *)kMenuToggleDebug, "label='toggle debug'");
	TwAddButton(_tweakbar, "wireframe", &App::tramp_menu, (void *)kMenuToggleWireframe, "label='toggle wireframe'");
	TwAddSeparator(_tweakbar, NULL, NULL);


	_debug_writer = new FontWriter();
	RETURN_ON_FAIL_BOOL_E(_debug_writer->init(System::instance().convert_path("data/fonts/TCB_____.ttf", System::kDirRelative), 0, 0, _width, _height));
	RETURN_ON_FAIL_BOOL_E(DebugMenu::instance().init());

	RETURN_ON_FAIL_BOOL_E(IMGui::instance().init());
	RETURN_ON_FAIL_BOOL_E(DebugRenderer::instance().init());
	//RETURN_ON_FAIL_BOOL_E(Network::instance().init());

	_test_effect = new TestEffect7();
	_test_effect->init();

	_trackball = new Trackball();
	_freefly = new FreeFlyCamera();

	init_menu();

	Profiler::instance().print();
	return true;
}

void App::on_quit()
{
	SendMessage(_hwnd, WM_DESTROY, 0, 0);
}

void App::init_menu()
{
	//DebugMenu::instance().add_button("quit", fastdelegate::MakeDelegate(this, &App::on_quit));
	//DebugMenu::instance().add_button("quit2", fastdelegate::MakeDelegate(this, &App::on_quit));
}

bool App::close()
{
	TwRemoveAllVars(tweakbar());
	TwDeleteAllBars();

	TwTerminate();

	if (_test_effect) {
		_test_effect->close();
		delete _test_effect;
	}

	RETURN_ON_FAIL_BOOL_E(Network::instance().close());
	RETURN_ON_FAIL_BOOL_E(DebugRenderer::instance().close());
	RETURN_ON_FAIL_BOOL_E(IMGui::instance().close());

	RETURN_ON_FAIL_BOOL_E(DebugMenu::instance().close());

	SAFE_DELETE(_debug_writer);
	SAFE_DELETE(_trackball);
	SAFE_DELETE(_freefly);

	RETURN_ON_FAIL_BOOL_E(Graphics::instance().close());
	RETURN_ON_FAIL_BOOL_E(System::instance().close());

	delete this;
	_instance = nullptr;

	return true;
}

void App::set_client_size()
{
	RECT client_rect;
	RECT window_rect;
	GetClientRect(_hwnd, &client_rect);
	GetWindowRect(_hwnd, &window_rect);
	window_rect.right -= window_rect.left;
	window_rect.bottom -= window_rect.top;
	window_rect.left = window_rect.top = 0;
	const int dx = window_rect.right - client_rect.right;
	const int dy = window_rect.bottom - client_rect.bottom;

	SetWindowPos(_hwnd, NULL, -1, -1, _width + dx, _height + dy, SWP_NOZORDER | SWP_NOREPOSITION);
}

bool App::create_window()
{
	const char* kClassName = "ShindigClass";

	WNDCLASSEXA wcex;
	ZeroMemory(&wcex, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEXA);
	wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc    = tramp_wnd_proc;
	wcex.hInstance      = _hinstance;
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName  = kClassName;


	RETURN_ON_FAIL_BOOL(RegisterClassExA(&wcex), LOG_WARNING_LN);

	//const UINT window_style = WS_VISIBLE | WS_POPUP | WS_OVERLAPPEDWINDOW;
	const UINT window_style = WS_VISIBLE | WS_POPUP;

	_hwnd = CreateWindowA(kClassName, "shindig - magnus österlind - 2010", window_style,
		CW_USEDEFAULT, CW_USEDEFAULT, _width, _height, NULL, NULL,
		_hinstance, NULL);

	set_client_size();

	ShowWindow(_hwnd, SW_SHOW);

	return true;
}

void App::tick()
{
}

void App::tramp_menu(void *menu_item)
{
	switch ((MenuItem)(int)menu_item) {
	case kMenuQuit:
		App::instance().on_quit();
		break;
	case kMenuToggleDebug:
		App::instance().toggle_debug();
		break;
	case kMenuToggleWireframe:
		App::instance().toggle_wireframe();
		break;
	case kMenuDrawXZPlane:
		App::instance().toggle_plane();
	}
}

void App::toggle_debug()
{
	DebugRenderer::instance().set_enabled(!DebugRenderer::instance().enabled());
}

void App::toggle_wireframe()
{
	_state_wireframe.value_changed(!_state_wireframe.value());
}

void App::toggle_plane()
{
	_draw_plane = !_draw_plane;
}


void App::run()
{
	auto& graphics = Graphics::instance();

	MSG msg = {0};

	float running_time = 0;
	const float dt = 1 / 100.0f;

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	LARGE_INTEGER cur;
	QueryPerformanceCounter(&cur);
	float cur_time = (float)(cur.QuadPart / freq.QuadPart);
	float accumulator = 0;


	while (WM_QUIT != msg.message) {
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			System::instance().tick();
			graphics.clear();
			_debug_writer->reset_frame();
			IMGui::instance().init_frame();

			DebugRenderer::instance().start_frame();

			if (_test_effect) {

				QueryPerformanceCounter(&cur);
				float new_time = (float)cur.QuadPart / freq.QuadPart;
				float delta_time = new_time - cur_time;
				cur_time = new_time;
				accumulator += delta_time;

				// calc the number of ticks to step
				int num_ticks = (int)(accumulator / dt);
				const float a = delta_time > 0 ? (accumulator - num_ticks * dt) / delta_time : 0;

				for (int i = 0; i < (int)_update_callbacks.size(); ++i)
					_update_callbacks[i](running_time, dt, num_ticks, a);

				running_time += num_ticks * dt;
				accumulator -= num_ticks * dt;

				{
					//Prof_Zone(render);
					_test_effect->render();

				}
			}

			if (_draw_plane) {
				D3DXPLANE plane;
				D3DXPlaneFromPointNormal(&plane, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
				DebugRenderer::instance().draw_plane(camera(), plane);
			}


			graphics.tick();
			_dbg_message_count = 0;
			add_dbg_message(".fps: %.1f\n", graphics.fps());

			int y_ofs = 100;

			_debug_writer->render();
			DebugMenu::instance().render();
			DebugRenderer::instance().end_frame();
			DebugRenderer::instance().render();
			IMGui::instance().render();

			TwDraw();

			Prof_update(1);

			Prof_Report *pob = Prof_create_report();

			graphics.present();

		}
	}

}

LRESULT App::tramp_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if( TwEventWin(hWnd, message, wParam, lParam) ) // send event message to AntTweakBar
		return 0;

	return App::instance().wnd_proc(hWnd, message, wParam, lParam);
}

LRESULT App::wnd_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	// See if the debug menu wants to handle the message first
	LRESULT res = DebugMenu::instance().wnd_proc(hWnd, message, wParam, lParam);

	UIState& ui_state = IMGui::instance().ui_state();

	switch( message ) 
	{
	case WM_SIZE:
		{
			const int width = LOWORD(lParam);
			const int height = HIWORD(lParam);
			Graphics::instance().resize(width, height);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		ui_state.mouse_down = 1;
		{
			MouseInfo m(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam));
			for (auto i = _mouse_down_callbacks.begin(), e = _mouse_down_callbacks.end(); i != e; ++i)
				(*i)(m);
		}
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ui_state.mouse_down = 0;
		{
			MouseInfo m(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam));
			for (auto i = _mouse_up_callbacks.begin(), e = _mouse_up_callbacks.end(); i != e; ++i)
				(*i)(m);
		}
		break;

	case WM_MOUSEMOVE:
		ui_state.mouse_x = GET_X_LPARAM(lParam);
		ui_state.mouse_y = GET_Y_LPARAM(lParam);
		{
			// only call the callbacks if we've actually moved
			static int last_wparam = 0;
			static int last_lparam = 0;
			if (last_lparam != lParam || last_wparam != wParam) {
				int x = LOWORD(lParam);
				int y = HIWORD(lParam);
				MouseInfo m(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), x, y, x - LOWORD(last_lparam), y - HIWORD(last_lparam), 0);
				for (auto i = _mouse_move_callbacks.begin(), e = _mouse_move_callbacks.end(); i != e; ++i)
					(*i)(m);
				last_lparam = lParam;
				last_wparam = wParam;
			}
		}
		break;

	case WM_MOUSEWHEEL:
		{
			MouseInfo m(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam), 0, 0, GET_WHEEL_DELTA_WPARAM(wParam));
			for (auto i = _mouse_wheel_callbacks.begin(), e = _mouse_wheel_callbacks.end(); i != e; ++i)
				(*i)(m);
		}
		break;

	case WM_KEYDOWN:
		{
			KeyInfo k(wParam, lParam);
			for (auto i = _keydown_callbacks.begin(), e = _keydown_callbacks.end(); i != e; ++i)
				(*i)(k);
		}
		break;

	case WM_CHAR:
		ui_state.key_entered = wParam;
		break;

	case WM_KEYUP:
		switch (wParam) 
		{
		case VK_ESCAPE:
			PostQuitMessage( 0 );
			break;
		default:
			{
				KeyInfo k(wParam, lParam);
				for (auto i = _keyup_callbacks.begin(), e = _keyup_callbacks.end(); i != e; ++i)
					(*i)(k);
			}
			break;
		}
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return res;
}


void App::add_mouse_move(const fnMouseMove& fn, bool add)
{
	if (add) {
		_mouse_move_callbacks.push_back(fn);
	} else {
		safe_erase(_mouse_move_callbacks, fn);
	}
}

void App::add_mouse_up(const fnMouseUp& fn, bool add)
{
	if (add) {
		_mouse_up_callbacks.push_back(fn);
	} else {
		safe_erase(_mouse_up_callbacks, fn);
	}
}

void App::add_mouse_down(const fnMouseDown& fn, bool add)
{
	if (add) {
		_mouse_down_callbacks.push_back(fn);
	} else {
		safe_erase(_mouse_down_callbacks, fn);
	}
}

void App::add_mouse_wheel(const fnMouseWheel& fn, bool add)
{
	if (add) {
		_mouse_wheel_callbacks.push_back(fn);
	} else {
		safe_erase(_mouse_wheel_callbacks, fn);
	}
}

void App::add_key_down(const fnKeyDown& fn, bool add)
{
	if (add) {
		_keydown_callbacks.push_back(fn);
	} else {
		safe_erase(_keydown_callbacks, fn);
	}
}

void App::add_key_up(const fnKeyUp& fn, bool add)
{
	if (add) {
		_keyup_callbacks.push_back(fn);
	} else {
		safe_erase(_keyup_callbacks, fn);
	}
}

void App::add_update_callback(const fnUpdate& fn, bool add)
{
	if (add)
		_update_callbacks.push_back(fn);
	else
		safe_erase(_update_callbacks, fn);
}

void App::add_dbg_message(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	const int len = _vscprintf(fmt, arg) + 1;

	char* buf = (char*)_alloca(len);
	vsprintf_s(buf, len, fmt, arg);
	va_end(arg);
	_debug_writer->write(0, _dbg_message_count++ * 16, 16, buf);
}

void App::find_app_root()
{
	// keep going up directory levels until we find "app.root", or we hit the bottom..
	char *starting_dir = _getcwd(NULL, 0);
	while (true) {
		if (file_exists("app.root")) {
			char *buf = _getcwd(NULL, 0);
			_app_root = buf;
			free(buf);
			return;
		}
		if (_chdir("..") == -1)
			break;
	}

	_app_root = starting_dir;
	free(starting_dir);
}


Camera *App::camera()
{
	switch (_cur_camera) {
	case 0 : return _trackball;
	case 1 : return _freefly;
	}
	return NULL;
}

