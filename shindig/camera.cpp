#include "stdafx.h"
#include "camera.hpp"
#include "debug_renderer.hpp"
#include "app.hpp"
#include <celsus/graphics.hpp>

using namespace fastdelegate;

D3DXMATRIX mtx(const D3DXVECTOR3& m0, const D3DXVECTOR3& m1, const D3DXVECTOR3& m2, const D3DXVECTOR3& m3)
{
	return D3DXMATRIX(
		m0.x, m0.y, m0.z, 0,
		m1.x, m1.y, m1.z, 0,
		m2.x, m2.y, m2.z, 0,
		m3.x, m3.y, m3.z, 1);
}

Camera::Camera()
	: _aspect(4/3.0f)
	, _fov(deg_to_rad(45))
	, _near_plane(0.1f)
	, _far_plane(1000)
{
	DebugRenderer::instance().add_debug_camera_delegate(MakeDelegate(this, &Camera::debug_camera), true);
}

Camera::~Camera()
{
	DebugRenderer::instance().add_debug_camera_delegate(MakeDelegate(this, &Camera::debug_camera), false);
}

D3DXMATRIX Camera::view() const
{
	// returns a matrix that transforms x into camera space
	D3DXMATRIX mtx;
	D3DXMatrixLookAtLH(&mtx, &_frame.e, &(_frame.e + _frame.z), &_frame.y);
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
	d->frame = _frame;
	d->fov;
	d->near_plane = _near_plane;
	d->far_plane = _far_plane;
}

void Camera::set_aspect_ratio(float aspect) 
{ 
  _aspect = aspect; 
  recalc();
}

void Camera::set_fov(float fov) 
{ 
  _fov = fov; 
  recalc();
}

void Camera::set_near_plane(float near_plane) 
{ 
  _near_plane = near_plane; 
  recalc();
}

void Camera::set_far_plane(float far_plane) 
{ 
  _far_plane = far_plane; 
  recalc();
}

FreeFlyCamera::FreeFlyCamera()
{
  init();

  App& app = App::instance();
  app.add_key_up(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_up), true);
  app.add_key_down(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_down), true);
  app.add_mouse_move(fastdelegate::MakeDelegate(this, &FreeFlyCamera::mouse_move), true);

  TwAddVarRO(app.tweakbar(), "freefly.x", TW_TYPE_DIR3F, &_frame.x, "label='x: ' axisx=x axisy=y axisz=-z");
  TwAddVarRO(app.tweakbar(), "freefly.y", TW_TYPE_DIR3F, &_frame.y, "label='y: ' axisx=x axisy=y axisz=-z");
  TwAddVarRO(app.tweakbar(), "freefly.z", TW_TYPE_DIR3F, &_frame.z, "label='z: ' axisx=x axisy=y axisz=-z");
  TwAddVarRW(app.tweakbar(), "freefly.phi", TW_TYPE_FLOAT, &_phi, "label='phi:'");
  TwAddVarRW(app.tweakbar(), "freefly.theta", TW_TYPE_FLOAT, &_theta, "label='theta:'");
  TwAddButton(app.tweakbar(), "freefly.reset", &FreeFlyCamera::reset, (void *)this, "label='reset'");

  update();
}

FreeFlyCamera::~FreeFlyCamera()
{
  App& app = App::instance();
  app.add_mouse_move(fastdelegate::MakeDelegate(this, &FreeFlyCamera::mouse_move), false);
  app.add_key_down(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_down), false);
  app.add_key_up(fastdelegate::MakeDelegate(this, &FreeFlyCamera::key_up), false);

  TwRemoveVar(app.tweakbar(), "freefly.x");
  TwRemoveVar(app.tweakbar(), "freefly.y");
  TwRemoveVar(app.tweakbar(), "freefly.z");
  TwRemoveVar(app.tweakbar(), "freefly.phi");
  TwRemoveVar(app.tweakbar(), "freefly.theta");
  TwRemoveVar(app.tweakbar(), "freefly.reset");
}

void FreeFlyCamera::init()
{
  _phi = 0;
  _theta = 0;
  _frame.e = D3DXVECTOR3(0,0,-100);
}

void FreeFlyCamera::update()
{
	D3DXMATRIX mtx, mtx2;
	D3DXMatrixRotationX(&mtx, _phi);
	D3DXMatrixRotationY(&mtx2, _theta);
	mtx = mtx * mtx2;
	D3DXVec3TransformCoord(&_frame.x, &D3DXVECTOR3(1,0,0), &mtx);
	D3DXVec3TransformCoord(&_frame.y, &D3DXVECTOR3(0,1,0), &mtx);
	D3DXVec3TransformCoord(&_frame.z, &D3DXVECTOR3(0,0,1), &mtx);
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
    _phi -= (float)D3DX_PI/10;
    break;
  case VK_DOWN:
    _phi += (float)D3DX_PI/10;
    break;
  case VK_LEFT:
    _theta -= (float)D3DX_PI/10;
    break;
  case VK_RIGHT:
    _theta += (float)D3DX_PI/10;
    break;

  case 'W':
		_frame.e += _frame.z;
		break;
  case 'S':
		_frame.e -= _frame.z;
		break;
	}

  _theta = fmodf(_theta, (float)D3DX_PI*2);
  _phi = fmodf(_phi, (float)D3DX_PI*2);

  update();

}

void FreeFlyCamera::key_up(const KeyInfo& k)
{

}

void FreeFlyCamera::mouse_move(const MouseInfo& m)
{
  float speed = 0.01f;
  if (m.left_down) {
    _theta += speed * m.x_delta;
    _phi += speed * m.y_delta;
    update();
  }
}

void FreeFlyCamera::reset(void *self)
{
  ((FreeFlyCamera *)self)->init();
  ((FreeFlyCamera *)self)->update();
}

ObjectCamera::ObjectCamera()
	: _alpha(0)
	, _theta(0)
{

}

Trackball::Trackball()
  : _prev_pos(kVec3Zero)
  , _rot(kQuatId)
  , _cam_pos(0,0,-100)
{
	reset((void *)this);

  App& app = App::instance();
  app.add_mouse_move(MakeDelegate(this, &Trackball::on_mouse_move), true);
  app.add_mouse_wheel(MakeDelegate(this, &Trackball::on_mouse_wheel), true);

  TwAddVarCB(app.tweakbar(), "trackball.rot", TW_TYPE_QUAT4F, &Trackball::cb_rot_set, &Trackball::cb_rot_get, (void *)this, "label='trackball rotate' axisx=x axisy=y axisz=-z");
  TwAddVarCB(app.tweakbar(), "trackball.fov", TW_TYPE_FLOAT, &Trackball::cb_fov_set, &Trackball::cb_fov_get, (void *)this, "label='FOV' min=0.2 max=1 step=0.1");
	TwAddButton(app.tweakbar(), "trackball.reset", &Trackball::reset, (void *)this, "label='reset trackball'");
	recalc();
}

Trackball::~Trackball()
{
  App& app = App::instance();
  app.add_mouse_move(MakeDelegate(this, &Trackball::on_mouse_move), false);
  app.add_mouse_wheel(MakeDelegate(this, &Trackball::on_mouse_wheel), false);
	TwRemoveVar(app.tweakbar(), "trackball rot");
}


void Trackball::cb_rot_set(const void *value, void *self)
{
		((Trackball*)self)->_rot = *(D3DXQUATERNION*)value;
		((Trackball*)self)->recalc();
}

void Trackball::cb_fov_set(const void *value, void *self)
{
  ((Trackball *)self)->set_fov(*(float *)value);
}

void Trackball::cb_fov_get(void *value, void *self)
{
  *(float *)value = ((Trackball *)self)->fov();
}

void Trackball::cb_rot_get(void *value, void *self)
{
	*(D3DXQUATERNION *)value = ((Trackball *)self)->_rot;
}

void Trackball::on_mouse_wheel(const MouseInfo& m)
{
	_cam_pos += m.wheel_delta / 100.0f * _dir;
	recalc();
}

void Trackball::on_mouse_move(const MouseInfo& m)
{
	Graphics& graphics = Graphics::instance();
  // convert from screen coord to -1..1
  float x, y, z;
  screen_to_clip((float)m.x, (float)m.y, (float)graphics.width(), (float)graphics.height(), &x, &y);

  // find the point under mouse
  float z2 = 1 - x*x - y*y;
  z = z2 > 0 ? sqrtf(z2) : 0;
  D3DXVECTOR3 v(x, y, z);

  if (m.left_down) {
    // calc axis & angle between prev and current pos
    const D3DXVECTOR3 axis = vec3_normalize(vec3_cross(v, _prev_pos));
    const float angle = acosf(vec3_dot(vec3_normalize(v), vec3_normalize(_prev_pos)));

    D3DXQUATERNION q;
    // we negate here because we want to rotate _into_ camera space
    D3DXQuaternionRotationAxis(&q, &axis, -angle);
    D3DXQuaternionMultiply(&_rot, &_rot, &q);
		recalc();
  }

  _prev_pos = v;
}

void Trackball::recalc()
{
	D3DXMATRIX mtx;
	D3DXMatrixRotationQuaternion(&mtx, &_rot);
	D3DXVECTOR3 rotated_axis[4];
	D3DXVECTOR3 default_axis[] = { D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,1,0), D3DXVECTOR3(0,0,1), -_cam_pos };
	D3DXVec3TransformCoordArray(rotated_axis, sizeof(D3DXVECTOR3), default_axis, sizeof(D3DXVECTOR3), &mtx, ELEMS_IN_ARRAY(default_axis));
	_view = matrix_from_vectors(rotated_axis[0], rotated_axis[1], rotated_axis[2], rotated_axis[3]);
	_right = matrix_get_row(_view, 0);
	_up = matrix_get_row(_view, 1);
	_dir = matrix_get_row(_view, 2);

  D3DXMatrixPerspectiveFovLH(&_proj, _fov, _aspect, _near_plane, _far_plane);
}

D3DXMATRIX Trackball::view() const
{
  return _view;
}

D3DXMATRIX Trackball::proj() const
{
  return _proj;
}

void Trackball::reset(void *self)
{
	Trackball *t = (Trackball *)self;
	t->_prev_pos = kVec3Zero;
	t->_rot = kQuatId;
	t->_cam_pos = D3DXVECTOR3(0,0,-100);
	t->recalc();
}
