#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"
#include "test_effect.hpp"
#include "test_effect2.hpp"
#include "test_effect3.hpp"

App* App::_instance = NULL;

App::App()
  : _hinstance(NULL)
  , _width(-1)
  , _height(-1)
  , _hwnd(NULL)
	, _test_effect(NULL)
{
}

App::~App()
{
}

App& App::instance()
{
	if (_instance == NULL) {
		_instance = new App();
	}
	return *_instance;
}

bool App::init(HINSTANCE hinstance)
{
	_hinstance = hinstance;
  _width = 800;
  _height = 600;
  create_window();
  RETURN_ON_FAIL_BOOL(System::instance().init(), LOG_ERROR_LN);
  RETURN_ON_FAIL_BOOL(Graphics::instance().init_directx(_hwnd, _width, _height), LOG_ERROR_LN);

	_test_effect = new TestEffect2();
	_test_effect->init();

	return true;
}

bool App::close()
{
  if (_test_effect) {
    _test_effect->close();
    delete _test_effect;
  }

  RETURN_ON_FAIL_BOOL(Graphics::instance().close(), LOG_ERROR_LN);
  RETURN_ON_FAIL_BOOL(System::instance().close(), LOG_ERROR_LN);
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
  wcex.lpfnWndProc    = wnd_proc;
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
  MSG msg = {0};
  while (WM_QUIT != msg.message) {
    if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
			System::instance().tick();
			Graphics::instance().clear();

			if (_test_effect) {
				_test_effect->render();
			}

			Graphics::instance().present();

    }
  }

}

LRESULT CALLBACK App::wnd_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
  PAINTSTRUCT ps;
  HDC hdc;

	App& a = App::instance();

  switch( message ) 
  {

  case WM_SIZE:
    {
      const int width = LOWORD(lParam);
      const int height = HIWORD(lParam);
			Graphics::instance().resize(width, height);
    }
    break;

  case WM_PAINT:
    hdc = BeginPaint( hWnd, &ps );
    //g_derived_system->render(); 
    EndPaint( hWnd, &ps );
    break;

  case WM_DESTROY:
    PostQuitMessage( 0 );
    break;

  case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		a._mouse_down_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		a._mouse_up_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam)));
		break;

	case WM_MOUSEMOVE:
		a._mouse_move_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam)));
    break;

	case WM_MOUSEWHEEL:
		a._mouse_wheel_signal(MouseInfo(!!(wParam & MK_LBUTTON), !!(wParam & MK_MBUTTON), !!(wParam & MK_RBUTTON), LOWORD(lParam), HIWORD(lParam), GET_WHEEL_DELTA_WPARAM(wParam)));
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
