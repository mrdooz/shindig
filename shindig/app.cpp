#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"
#include "test_effect.hpp"
#include "test_effect2.hpp"
#include "test_effect3.hpp"
#include "test_effect4.hpp"
#include "font.hpp"
#include <celsus/file_utils.hpp>
#include "debug_writer.hpp"
#include "debug_menu.hpp"

App* App::_instance = NULL;



App::App()
  : _hinstance(NULL)
  , _width(-1)
  , _height(-1)
  , _hwnd(NULL)
	, _test_effect(NULL)
	, _debug_writer(nullptr)
{
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
  _width = 800;
  _height = 600;
  create_window();
  RETURN_ON_FAIL_BOOL_E(System::instance().init());
  RETURN_ON_FAIL_BOOL_E(Graphics::instance().init_directx(_hwnd, _width, _height));
	_debug_writer = new DebugWriter();
	RETURN_ON_FAIL_BOOL_E(_debug_writer->init(_width, _height, 30));
	RETURN_ON_FAIL_BOOL_E(DebugMenu::instance().init());

	_test_effect = new TestEffect3();
	_test_effect->init();

  init_menu();

	return true;
}

void App::on_quit()
{
	SendMessage(_hwnd, WM_DESTROY, 0, 0);
}

void App::init_menu()
{
	DebugMenu::instance().add_button("quit", fastdelegate::MakeDelegate(this, &App::on_quit));
	DebugMenu::instance().add_button("quit2", fastdelegate::MakeDelegate(this, &App::on_quit));
}

bool App::close()
{
  if (_test_effect) {
    _test_effect->close();
    delete _test_effect;
  }

	_debug_writer->close();
	SAFE_DELETE(_debug_writer);
	RETURN_ON_FAIL_BOOL_E(DebugMenu::instance().close());
  RETURN_ON_FAIL_BOOL_E(Graphics::instance().close());
  RETURN_ON_FAIL_BOOL_E(System::instance().close());
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
  const char* kClassName = "AppClass";

  WNDCLASSEXA wcex;
  ZeroMemory(&wcex, sizeof(wcex));

  wcex.cbSize = sizeof(WNDCLASSEXA);
  wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc    = tramp_wnd_proc;
  wcex.hInstance      = _hinstance;
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszClassName  = kClassName;


  RETURN_ON_FAIL_BOOL(RegisterClassExA(&wcex), LOG_WARNING_LN);

  const uint32_t window_style = WS_VISIBLE | WS_POPUP | WS_OVERLAPPEDWINDOW;

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

void App::run()
{
  auto& graphics = Graphics::instance();

  MSG msg = {0};
  while (WM_QUIT != msg.message) {
    if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
			System::instance().tick();
			graphics.clear();
			_debug_writer->reset_frame();

			if (_test_effect) {
				_test_effect->render();
			}

      graphics.tick();
      add_dbg_message(".fps: %.1f\n", graphics.fps());

			_debug_writer->render();
			DebugMenu::instance().render();

			graphics.present();

    }
  }

}

LRESULT App::tramp_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return App::instance().wnd_proc(hWnd, message, wParam, lParam);
}

LRESULT App::wnd_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
	// See if the debug menu wants to handle the message first
	LRESULT res = DebugMenu::instance().wnd_proc(hWnd, message, wParam, lParam);
	if (res != 0)
		return res;

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
		_mouse_down_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		_mouse_up_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_MOUSEMOVE:
		_mouse_move_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam)));
    break;

	case WM_MOUSEWHEEL:
		_mouse_wheel_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam), GET_WHEEL_DELTA_WPARAM(wParam)));
		break;

  case WM_KEYDOWN:
		//App::instance()._key_signal(IoKey(wParam, lParam));
		break;

  case WM_KEYUP:
    switch (wParam) 
    {
    case VK_ESCAPE:
      PostQuitMessage( 0 );
      break;
		default:
			//App::instance()._key_signal(IoKey(wParam, lParam));
			break;
    }
    break;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
  }
  return 0;
}

sig2::connection App::add_mouse_move(const fnMouseMove& slot)
{
	return _mouse_move_signal.connect(slot);
}

sig2::connection App::add_mouse_up(const fnMouseUp& slot)
{
	return _mouse_up_signal.connect(slot);
}

sig2::connection App::add_mouse_down(const fnMouseDown& slot)
{
	return _mouse_down_signal.connect(slot);
}

sig2::connection App::add_mouse_wheel(const fnMouseWheel& slot)
{
	return _mouse_wheel_signal.connect(slot);
}

void App::add_dbg_message(const char* fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);

  const int len = _vscprintf(fmt, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, fmt, arg);
  va_end(arg);
  _debug_writer->write(0, 0, buf, 10, 10);
}
