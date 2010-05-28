#include "stdafx.h"
#include "test_effect3.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include "dynamic_vb.hpp"
#include <celsus/text_scanner.hpp>
#include <celsus/D3D11Descriptions.hpp>
#include <celsus/math_utils.hpp>
#include "obj_loader.hpp"
#include "mesh2.hpp"
#include "app.hpp"


TestEffect3::TestEffect3()
  : _mesh(NULL)  
  , _effect(NULL)
	, _cam_radius(150)
  , _alpha(0)
  , _theta(0)
  , _first_update(true)
{
}

TestEffect3::~TestEffect3()
{
  SAFE_DELETE(_effect);
  SAFE_DELETE(_mesh);
}

bool TestEffect3::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();

	App::instance().add_mouse_move(MakeDelegate(this, &TestEffect3::on_mouse_move));
	App::instance().add_mouse_up(MakeDelegate(this, &TestEffect3::on_mouse_up));
	App::instance().add_mouse_down(MakeDelegate(this, &TestEffect3::on_mouse_down));
	App::instance().add_mouse_wheel(MakeDelegate(this, &TestEffect3::on_mouse_wheel));
  
  float v = 0.5f;
  g.set_clear_color(D3DXCOLOR(v, v, v, 1));

  RETURN_ON_FAIL_BOOL(
    s.add_file_changed(s.convert_path("sculptris/blob1.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true),
    ErrorPredicate<bool>, LOG_ERROR_LN);

  RETURN_ON_FAIL_BOOL(r.load_shaders(s.convert_path("effects/sculptris_1.fx", System::kDirRelative).c_str(), "vsMain", "psMain", 
    MakeDelegate(this, &TestEffect3::effect_loaded)), ErrorPredicate<bool>, LOG_ERROR_LN);

  _dss.Attach(rt::D3D11::DepthStencilDescription().Create(d));

  return true;
}

bool TestEffect3::close()
{
  return true;
}

D3DXVECTOR3 TestEffect3::calc_cam_pos() const
{
  return D3DXVECTOR3(
		_cam_radius * sinf(_theta) * sinf(_alpha),
		_cam_radius * cosf(_theta),
		_cam_radius * sinf(_theta) * cosf(_alpha)
    );
}

bool TestEffect3::render()
{
  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  float blend_factors[] = {1, 1, 1, 1};
  context->OMSetDepthStencilState(_dss, 0);


  D3DXMATRIX view, proj;
  D3DXMatrixLookAtLH(&view, &(_mesh->_bounding_center + calc_cam_pos()), &_mesh->_bounding_center, &D3DXVECTOR3(0,1,0));
  D3DXMatrixPerspectiveFovLH(&proj, deg_to_rad(45), 4 / 3.0f, 1, 1000);
  D3DXMATRIX mtx;
  D3DXMatrixTranspose(&mtx, &(view * proj));
  _effect->set_vs_variable("mtx", mtx);
  _effect->set_cbuffer();

  _effect->set_shaders(context);

  _mesh->render(context);

  return true;
}

bool TestEffect3::load_mesh(const std::string& filename)
{
  SAFE_DELETE(_mesh);

  ObjLoader loader;
  bool res = loader.load_from_file(filename.c_str(), &_mesh);

	float r = _mesh->_bounding_radius;
	float fov = deg_to_rad(45) / (4/3.0f);
	float x = atanf(fov);
	float a = (r - r * x) / x;
	_cam_radius = r + a;

	return res;
}

void TestEffect3::effect_loaded(EffectWrapper *effect)
{
  SAFE_DELETE(_effect);
  _effect = effect;
	_mesh->set_layout(_effect->create_input_layout(_mesh->input_desc()));
}

void TestEffect3::on_mouse_move(const MouseInfo& info)
{
  if (_first_update) {
    _first_update = false;
    _prev_mouse = info;
    return;
  }

  // do stuff
  if (info.left_down) {
    const float delta = (float)(info.time - _prev_mouse.time);
    if (delta > 0) {
      _alpha += (info.x - _prev_mouse.x) / delta;
      _theta -= (info.y - _prev_mouse.y) / delta;
    }
  }

  _prev_mouse = info;
}

void TestEffect3::on_mouse_up(const MouseInfo& info)
{
  if (_first_update) {
    _first_update = false;
    _prev_mouse = info;
    return;
  }

  // do stuff

  _prev_mouse = info;
}

void TestEffect3::on_mouse_down(const MouseInfo& info)
{
  if (_first_update) {
    _first_update = false;
    _prev_mouse = info;
    return;
  }

  // do stuff

  _prev_mouse = info;

}

void TestEffect3::on_mouse_wheel(const MouseInfo& info)
{
	_cam_radius += info.wheel_delta;		
}
