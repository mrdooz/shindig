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
	App::instance().add_key_up(fastdelegate::MakeDelegate(this, &Camera::key_up), true);
	App::instance().add_key_down(fastdelegate::MakeDelegate(this, &Camera::key_down), true);
	App::instance().add_mouse_move(fastdelegate::MakeDelegate(this, &Camera::mouse_move), true);
}

Camera::~Camera()
{
	App::instance().add_key_up(fastdelegate::MakeDelegate(this, &Camera::key_up), false);
	App::instance().add_key_down(fastdelegate::MakeDelegate(this, &Camera::key_down), false);
	App::instance().add_mouse_move(fastdelegate::MakeDelegate(this, &Camera::mouse_move), false);
	DebugRenderer::instance().add_debug_camera_delegate(fastdelegate::MakeDelegate(this, &Camera::debug_camera), false);
}

D3DXMATRIX Camera::view() const
{
	// returns a matrix that transforms x into camera space
	return mtx(_frame.x, _frame.y, _frame.z, -_frame.e);
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

void Camera::key_down(const KeyInfo& k)
{
//	D3DXVECTOR3 dir = vec3_normalize(_lookat - _pos);
	switch (k.key) {
	case VK_UP:
		_frame.e += _frame.z;
	//	_pos += dir;
		break;
	case VK_DOWN:
		_frame.e -= _frame.z;
		//_pos -= dir;
		break;
	}

}

void Camera::key_up(const KeyInfo& k)
{

}

void Camera::mouse_move(const MouseInfo& m)
{

}

ObjectCamera::ObjectCamera()
	: _alpha(0)
	, _theta(0)
{

}
