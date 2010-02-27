#ifndef _APP_HPP_
#define _APP_HPP_

#include <stdint.h>

class System;
class App
{
public:
  App(HINSTANCE instance);
  ~App();

  void init();
  void run();

private:
  void create_window();
  void set_client_size();
  static LRESULT CALLBACK wnd_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

  HINSTANCE _instance;
  System *_system;
  int32_t _width;
  int32_t _height;
  HWND _hwnd;

};

#endif
