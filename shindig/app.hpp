#ifndef _APP_HPP_
#define _APP_HPP_

#include <stdint.h>
#include <MMSystem.h>
#include <celsus/string_utils.hpp>

class EffectBase;
class FontWriter;

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

template<class T>
struct AppState
{
  typedef fastdelegate::FastDelegate2<const string2&, const T&, void> fnCallback;
  typedef std::vector<fnCallback> Callbacks;

  AppState(const string2& name, const T& value) : _name(name), _value(value) {}

  void add_callback(const fnCallback& fn)
  {
    _callbacks.push_back(fn);
    // call the newly added callback with the default value
    fn(_name, _value);
  }

  void remove_callback(const fnCallback& fn)
  {
    auto it = std::find(_callbacks.begin(), _callbacks.end(), fn);
    if (it != _callbacks.end())
      _callbacks.erase(it);
  }

  void value_changed(const T& new_value)
  {
    _value = new_value;
    for (auto i = _callbacks.begin(), e = _callbacks.end(); i != e; ++i) {
      (*i)(_name, _value);
    }
  }

  string2 _name;
  T _value;
  Callbacks _callbacks;
};

#define ADD_APP_STATE(type, name) \
  public: \
  void add_appstate_callback(const AppState<type>::fnCallback& fn) { _state_ ## name.add_callback(fn); }  \
  void remove_appstate_callback(const AppState<type>::fnCallback& fn) { _state_ ## name.remove_callback(fn); }  \
  private:  \
    AppState<type> _state_ ## name; \
  public:\

class App
{
public:

	static App& instance();

	bool init(HINSTANCE hinstance);
	bool close();

	void	tick();

  void add_dbg_message(const char* fmt, ...);

  void run();

	void add_mouse_move(const fnMouseMove& fn);
	void add_mouse_up(const fnMouseUp& fn);
	void add_mouse_down(const fnMouseDown& fn);
	void add_mouse_wheel(const fnMouseWheel& fn);

  ADD_APP_STATE(bool, wireframe);
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

  std::vector< std::function<void (const MouseInfo&)> > _mouse_move_signal;
	std::vector< std::function<void (const MouseInfo&)> > _mouse_up_signal;
	std::vector< std::function<void (const MouseInfo&)> > _mouse_down_signal;
	std::vector< std::function<void (const MouseInfo&)> > _mouse_wheel_signal;

	FontWriter *_debug_writer;

};

#endif
