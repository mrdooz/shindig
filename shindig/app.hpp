#ifndef _APP_HPP_
#define _APP_HPP_

#include <stdint.h>

struct TestEffect;

class App
{
public:

	static App& instance();

	bool init(HINSTANCE hinstance);
	bool close();

  void run();
private:
	DISALLOW_COPY_AND_ASSIGN(App);
	App();
	~App();

  bool create_window();
  void set_client_size();
  static LRESULT CALLBACK wnd_proc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	static App* _instance;
	TestEffect* _test_effect;
  HINSTANCE _hinstance;
  int32_t _width;
  int32_t _height;
  HWND _hwnd;

};

#endif
