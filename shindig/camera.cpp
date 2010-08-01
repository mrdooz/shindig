#include "stdafx.h"
#include "camera.hpp"
#include "debug_renderer.hpp"
#include "app.hpp"

D3DXMATRIX mtx(const D3DXVECTOR3& m0, const D3DXVECTOR3& m1, const D3DXVECTOR3& m2, const D3DXVECTOR3& m3)
{
	return D3DXMATRIX(
		m0.x, m0.y, m0.z, 0,
		m1.x, m1.y, m1.z, 0,
		m2.x, m2.y, m2.z, 0,
		m3.x, m3.y, m3.z, 1);
}

Camera::Camera()
//	: _pos(0,0,-100)
//	, _lookat(0,0,0)
//	, _up(0,1,0)
	: _aspect(4/3.0f)
	, _fov(deg_to_rad(45))
	, _near_plane(1)
	, _far_plane(1000)
{
	DebugRenderer::instance().add_debug_camera_delegate(fastdelegate::MakeDelegate(this, &Camera::debug_camera), true);
}

Camera::~Camera()
{
	DebugRenderer::instance().add_debug_camera_delegate(fastdelegate::MakeDelegate(this, &Camera::debug_camera), false);
}

D3DXMATRIX Camera::view() const
{
	// returns a matrix that transforms x into camera space
	D3DXMATRIX mtx;
	D3DXMatrixLookAtLH(&mtx, &_frame.e, &(_frame.e + _frame.z), &_frame.y);
	return mtx;
	//return mtx(_frame.x, _frame.y, _frame.z, -_frame.e);
}

D3DXMATRIX Camera::proj() const
{
	D3DXMATRIX mtx;
	D3DXMatrixPerspectiveFovLH(&mtx, _fov, _aspect, _near_plane, _far_plane);
	return mtx;
}

void Camera::debug_camera(DebugCamera *d)
{
	d->frame = _frame;
	d->fov;
	d->near_plane = _near_plane;
	d->far_plane = _far_plane;
}

FreeFlyCamera::FreeFlyCamera()
  : _phi((float)D3DX_PI/2)
  , _theta((float)D3DX_PI/2)
{
  App::instance().add_key_up(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_up), true);
  App::instance().add_key_down(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_down), true);
  App::instance().add_mouse_move(fastdelegate::MakeDelegate(this, &FreeFlyCamera::mouse_move), true);
  update();
}

FreeFlyCamera::~FreeFlyCamera()
{
  App::instance().add_mouse_move(fastdelegate::MakeDelegate(this, &FreeFlyCamera::mouse_move), false);
  App::instance().add_key_down(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_down), false);
  App::instance().add_key_up(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_up), false);
}

void FreeFlyCamera::update()
{
  D3DXVECTOR3 tmp;
  spherical_to_cart(0, 0, 1, &tmp);
  spherical_to_cart(D3DX_PI/2, 0, 1, &tmp);
  spherical_to_cart(0, D3DX_PI/2, 1, &tmp);
  spherical_to_cart(D3DX_PI/2, D3DX_PI/2, 1, &tmp);

  Frame f;
  f.e = _frame.e;
  // calc the dir from the spherical coordinates
  spherical_to_cart(_phi, _theta, 1, &f.z);
  f.x = vec3_cross(_frame.y, f.z);
  f.y = vec3_cross(f.z, f.x);
  _frame = f;
}

void FreeFlyCamera::tick()
{
  App::instance().add_dbg_message("phi: %f, theta: %f", _phi, _theta);
  App::instance().add_dbg_message("i: %f, %f, %f", _frame.x.x, _frame.x.y, _frame.x.z);
  App::instance().add_dbg_message("j: %f, %f, %f", _frame.y.x, _frame.y.y, _frame.y.z);
  App::instance().add_dbg_message("k: %f, %f, %f", _frame.z.x, _frame.z.y, _frame.z.z);
}


void FreeFlyCamera::key_down(const KeyInfo& k)
{
//	D3DXVECTOR3 dir = vec3_normalize(_lookat - _pos);
	switch (k.key) {
	case VK_UP:
  case 'A':
		_frame.e += _frame.z;
	//	_pos += dir;
		break;
	case VK_DOWN:
  case 'Z':
		_frame.e -= _frame.z;
		//_pos -= dir;
		break;
	}

}

void FreeFlyCamera::key_up(const KeyInfo& k)
{

}

void FreeFlyCamera::mouse_move(const MouseInfo& m)
{
  float speed = 0.01f;
  if (m.left_down) {
    _theta -= speed * m.x_delta;
    _phi += speed * m.y_delta;
    update();
  }
}

ObjectCamera::ObjectCamera()
	: _alpha(0)
	, _theta(0)
{

}
