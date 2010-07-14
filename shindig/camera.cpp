#include "stdafx.h"
#include "camera.hpp"
#include "debug_renderer.hpp"

Camera::Camera()
	: _pos(0,0,-100)
	, _lookat(0,0,0)
	, _up(0,1,0)
	, _aspect(4/3.0f)
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
	D3DXMATRIX mtx;
	D3DXMatrixLookAtLH(&mtx, &_pos, &_lookat, &_up);
	return mtx;
}

D3DXMATRIX Camera::proj() const
{
	D3DXMATRIX mtx;
	D3DXMatrixPerspectiveFovLH(&mtx, _fov, _aspect, _near_plane, _far_plane);
	return mtx;
}

void Camera::debug_camera(DebugCamera *d)
{
	d->pos = _pos;
	d->lookat = _lookat;
	d->up	= _up;

	d->fov;
	d->near_plane = _near_plane;
	d->far_plane = _far_plane;
}

ObjectCamera::ObjectCamera()
	: _alpha(0)
	, _theta(0)
{

}