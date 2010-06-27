#ifndef _APP_HPP_
#define _APP_HPP_

#include <stdint.h>
#include <MMSystem.h>

class EffectBase;
class DebugWriter;

struct MouseInfo
{
  MouseInfo() : left_down(false), middle_down(false), right_down(false), x(0), y(0), wheel_delta(0), time(timeGetTime()) {}
	MouseInfo(const bool l, const bool m, const bool r, const int x, const int y, const int wheel_delta = 0) 
		: left_down(l), middle_down(m), right_down(r), x(x), y(y), wheel_delta(wheel_delta), time(timeGetTime()) {}
	bool	left_down;
	bool	middle_down;
	bool	right_down;

	int		x;
	int		y;
	int		wheel_delta;
	DWORD	time;
};

typedef fastdelegate::FastDelegate1<const MouseInfo&> fnMouseMove;
typedef fastdelegate::FastDelegate1<const MouseInfo&> fnMouseUp;
typedef fastdelegate::FastDelegate1<const MouseInfo&> fnMouseDown;
typedef fastdelegate::FastDelegate1<const MouseInfo&> fnMouseWheel;

namespace sig2 = boost::signals2;

class App
{
public:

	static App& instance();

	bool init(HINSTANCE hinstance);
	bool close();

	void	tick();

  void add_dbg_message(const char* fmt, ...);

  void run();

	sig2::connection add_mouse_move(const fnMouseMove& slot);
	sig2::connection add_mouse_up(const fnMouseUp& slot);
	sig2::connection add_mouse_down(const fnMouseDown& slot);
	sig2::connection add_mouse_wheel(const fnMouseWheel& slot);

private:
	DISALLOW_COPY_AND_ASSIGN(App);
	App();
	~App();

	void on_quit();
	void init_menu();
  bool create_window();
  void set_client_size();

	LRESULT wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK tramp_wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static App* _instance;
	EffectBase* _test_effect;
  HINSTANCE _hinstance;
  int32_t _width;
  int32_t _height;
  HWND _hwnd;

	CRITICAL_SECTION _cs_queue;

	sig2::signal<void (const MouseInfo&)> _mouse_move_signal;
	sig2::signal<void (const MouseInfo&)> _mouse_up_signal;
	sig2::signal<void (const MouseInfo&)> _mouse_down_signal;
	sig2::signal<void (const MouseInfo&)> _mouse_wheel_signal;

	DebugWriter *_debug_writer;
};

#endif
