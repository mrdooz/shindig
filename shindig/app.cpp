#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"
#include "graphics.hpp"
#include "test_effect.hpp"

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
  RETURN_ON_FAIL_BOOL(System::instance().init(), ErrorPredicate<bool>, LOG_ERROR_LN);
  RETURN_ON_FAIL_BOOL(Graphics::instance().init(_hwnd, _width, _height), ErrorPredicate<bool>, LOG_ERROR_LN);

	_test_effect = new TestEffect();
	_test_effect->init();

	return true;
}

bool App::close()
{
	_test_effect->close();
	delete _test_effect;

  RETURN_ON_FAIL_BOOL(Graphics::instance().close(), ErrorPredicate<bool>, LOG_ERROR_LN);
  RETURN_ON_FAIL_BOOL(System::instance().close(), ErrorPredicate<bool>, LOG_ERROR_LN);
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


  RETURN_ON_FAIL_BOOL(RegisterClassExA(&wcex), ErrorPredicate<int>, LOG_WARNING_LN);

  const uint32_t window_style = WS_VISIBLE | WS_POPUP | WS_OVERLAPPEDWINDOW;

  _hwnd = CreateWindowA(kClassName, "shindig - magnus österlind - 2010", window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, _width, _height, NULL, NULL,
    _hinstance, NULL);

  set_client_size();

  ShowWindow(_hwnd, SW_SHOW);

	return true;
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

			_test_effect->render();

			Graphics::instance().present();

    }
  }

}

LRESULT CALLBACK App::wnd_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{
  PAINTSTRUCT ps;
  HDC hdc;

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
    //g_derived_system->input_.left_button_down = true;
    break;

  case WM_LBUTTONUP:
    ///g_derived_system->input_.left_button_down = false;
    break;

  case WM_RBUTTONDOWN:
    //g_derived_system->input_.right_button_down = true;
    break;

  case WM_RBUTTONUP:
    //g_derived_system->input_.right_button_down  = false;
    break;

  case WM_MOUSEMOVE:
    {
      /*
      static bool first_movement = true;
      if (!first_movement) {
      g_derived_system->input_.last_x_pos = g_derived_system->input_.x_pos;
      g_derived_system->input_.last_y_pos = g_derived_system->input_.y_pos;
      }
      g_derived_system->input_.x_pos = LOWORD(lParam);
      g_derived_system->input_.y_pos = HIWORD(lParam);
      if (first_movement) {
      g_derived_system->input_.last_x_pos = g_derived_system->input_.x_pos;
      g_derived_system->input_.last_y_pos = g_derived_system->input_.y_pos;
      first_movement = false;
      }
      */
    }
    break;

  case WM_KEYDOWN:
    //g_derived_system->input_.key_status[wParam & 255] = true;
    break;

  case WM_KEYUP:
    switch (wParam) 
    {

    case VK_ESCAPE:
      PostQuitMessage( 0 );
      break;

    }
    break;

  default:
    return DefWindowProc( hWnd, message, wParam, lParam );
  }
  return 0;

}

