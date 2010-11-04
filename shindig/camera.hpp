#pragma once
#include <celsus/math_utils.hpp>

struct DebugCamera;
struct KeyInfo;
struct MouseInfo;

class Camera
{
public:
	Camera();
	virtual ~Camera() = 0;

	virtual D3DXMATRIX view() const;
	virtual D3DXMATRIX proj() const;
  virtual void tick() {}

protected:
	Frame _frame;
	float _aspect, _fov;
	float _near_plane, _far_plane;

private:

	void debug_camera(DebugCamera *d);
};

class FreeFlyCamera : public Camera
{
public:
  FreeFlyCamera();
  virtual ~FreeFlyCamera();
  void mouse_move(const MouseInfo& m);
  void key_down(const KeyInfo& k);
  void key_up(const KeyInfo& k);

  virtual void tick();
private:
  void update();
  float _phi, _theta;
};

class ObjectCamera : public Camera
{
public:
	ObjectCamera();
private:
	float _alpha, _theta;
};

// Trackball, from http://viewport3d.com/trackball.htm
class Trackball : public Camera
{
public:
  Trackball();
  ~Trackball();
  void on_mouse_move(const MouseInfo& m);
  void on_mouse_wheel(const MouseInfo& m);
  virtual D3DXMATRIX view() const;
  virtual D3DXMATRIX proj() const;
private:
	static void __stdcall cb_rot_set(const void *value, void *self);
	static void __stdcall cb_rot_get(void *value, void *self);
	static void __stdcall reset(void *self);
	void recalc();
  D3DXMATRIX _view;
  D3DXVECTOR3 _prev_pos;
  D3DXVECTOR3 _cam_pos;
  D3DXQUATERNION _rot;

};