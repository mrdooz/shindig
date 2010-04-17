#ifndef _APP_HPP_
#define _APP_HPP_

#include <stdint.h>

struct TestEffect;

// holds a WM_KEY[DOWN|UP] message
struct IoKey
{
	IoKey(const int a, const int b) : key_code(a), state(b) {}
	int	key_code;
	int	state;
};

struct IoMouse
{
	enum Event 
	{ 
		kLeftButtonDown, 
		kLeftButtonUp, 
		kMiddleButtonDown,
		kMiddleButtonUp,
		kRightButtonDown,
		kRightButtonUp
	};

	IoMouse(const Event event, const int16_t x, const int16_t y) : event(event), x(x), y(y) {}
	Event event;
	int16_t	x;
	int16_t	y;
};

class App
{
public:

	static App& instance();

	bool init(HINSTANCE hinstance);
	bool close();

	void	tick();

  void run();
private:
	DISALLOW_COPY_AND_ASSIGN(App);
	App();
	~App();

  bool create_window();
  void set_client_size();

	void	key_slot(const IoKey& k);
	void	mouse_slot(const IoMouse& m);

  static LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static App* _instance;
	TestEffect* _test_effect;
  HINSTANCE _hinstance;
  int32_t _width;
  int32_t _height;
  HWND _hwnd;

	CRITICAL_SECTION _cs_queue;
	std::vector<IoKey> _key_buffer;
	std::vector<IoMouse> _mouse_buffer;

	boost::signals2::signal<void (const IoKey&)> _key_signal;
	boost::signals2::signal<void (const IoMouse&)> _mouse_signal;
};

#endif
