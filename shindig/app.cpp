#include "stdafx.h"
#include "app.hpp"
#include "system.hpp"

namespace
{
  App* g_app = NULL;
}

App::App(HINSTANCE instance)
  : _instance(instance)
  , _system(NULL)
  , _width(-1)
  , _height(-1)
  , _hwnd(NULL)
{
  g_app = this;
}

App::~App()
{
  SAFE_DELETE(_system);
}

void App::init()
{
  _width = 800;
  _height = 600;
  create_window();
  _system = new System();
  _system->init_directx(_hwnd, _width, _height);
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

void App::create_window()
{
  const char* kClassName = "AppClass";

  WNDCLASSEXA wcex;
  ZeroMemory(&wcex, sizeof(wcex));

  wcex.cbSize = sizeof(WNDCLASSEXA);
  wcex.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc    = wnd_proc;
  wcex.hInstance      = _instance;
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszClassName  = kClassName;

  if (!RegisterClassExA(&wcex)) {
    LOG_ERROR_LN("Error registering class");
    return;
    //return false;
  }

  const uint32_t window_style = WS_VISIBLE | WS_POPUP | WS_OVERLAPPEDWINDOW;

  _hwnd = CreateWindowA(kClassName, "shindig - magnus österlind - 2010", window_style,
    CW_USEDEFAULT, CW_USEDEFAULT, _width, _height, NULL, NULL,
    _instance, NULL);

  set_client_size();

  ShowWindow(_hwnd, SW_SHOW);

}

void App::run()
{
  MSG msg = {0};
  while (WM_QUIT != msg.message) {
    if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    } else {
      _system->tick();
      //render();
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
      if (g_app->_system) {
        g_app->_system->resize(width, height);
      }
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

