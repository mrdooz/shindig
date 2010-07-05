#include "stdafx.h"
#include "test_effect3.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include "dynamic_vb.hpp"
#include <celsus/text_scanner.hpp>
#include <celsus/file_utils.hpp>
#include <celsus/D3D11Descriptions.hpp>
#include <celsus/math_utils.hpp>
#include "obj_loader.hpp"
#include "mesh2.hpp"
#include "app.hpp"
#include "material.hpp"
#include "debug_menu.hpp"


TestEffect3::TestEffect3()
  : _effect(NULL)
	, _cam_radius(150)
  , _alpha(0)
  , _theta(0)
  , _first_update(true)
	, _test(1.2f, 2.3f, 4.5f)
{
}

TestEffect3::~TestEffect3()
{
  SAFE_DELETE(_effect);
	container_delete(_meshes);
}

bool TestEffect3::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();

	DebugMenu::instance().add_label("mr tjong!", &_test);

	App::instance().add_mouse_move(MakeDelegate(this, &TestEffect3::on_mouse_move));
	App::instance().add_mouse_up(MakeDelegate(this, &TestEffect3::on_mouse_up));
	App::instance().add_mouse_down(MakeDelegate(this, &TestEffect3::on_mouse_down));
	App::instance().add_mouse_wheel(MakeDelegate(this, &TestEffect3::on_mouse_wheel));
  
  float v = 0.5f;
  g.set_clear_color(D3DXCOLOR(v, v, v, 1));

  //RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("sculptris/blob1.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true));

  //RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scenes/sponza_obj/sponza.obj", System::kDirDropBox), 
    //MakeDelegate(this, &TestEffect3::load_mesh), true));

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("C:/Users/dooz/Downloads/PC-03_ON_OFF_Redux (1)/PC-03_Redux/Assets/Models/Scene1_Intro_Cube.obj", System::kDirAbsolute), 
		MakeDelegate(this, &TestEffect3::load_mesh), true));

/*
	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scenes/sponza_obj/sponza.mtl", System::kDirDropBox), 
		MakeDelegate(this, &TestEffect3::load_material), true));
*/
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/sponza.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
    MakeDelegate(this, &TestEffect3::effect_loaded)));

	_dss.Attach(rt::D3D11::DepthStencilDescription().Create(d));
	_blend_state.Attach(rt::D3D11::BlendDescription().Create(d));

	_sampler_state.Attach(rt::D3D11::SamplerDescription().
		AddressU_(D3D11_TEXTURE_ADDRESS_CLAMP).
		AddressV_(D3D11_TEXTURE_ADDRESS_CLAMP).
		Filter_(D3D11_FILTER_MIN_MAG_MIP_LINEAR).
		Create(d));


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

	float blend_factor[] = { 1, 1, 1, 1 };
	context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);

  D3DXMATRIX view, proj;
	D3DXMatrixLookAtLH(&view, &(_meshes[0]->_bounding_center + calc_cam_pos()), &_meshes[0]->_bounding_center, &D3DXVECTOR3(0,1,0));
	D3DXMatrixPerspectiveFovLH(&proj, deg_to_rad(45), 4 / 3.0f, 1, 5000 /*_meshes[0]->_bounding_radius*/);
	D3DXMATRIX mtx;
	D3DXMatrixTranspose(&mtx, &(view * proj));
	_effect->set_vs_variable("mtx", mtx);
	_effect->set_cbuffer();
	_effect->set_shaders(context);

	ID3D11SamplerState *samplers[] = { _sampler_state };
	context->PSSetSamplers(0, 1, samplers);

	ID3D11ShaderResourceView* t[] = { 0, 0 };

	for (int i = 0; i < (int)_meshes.size(); ++i) {
		Mesh2 *mesh = _meshes[i];
		if (Material *material = _materials[mesh->_material_name]) {
			t[0] = _textures[material->string_values["map_Kd"]];
			Material::StringValues::iterator it = material->string_values.find("map_d");
			t[1] = it != material->string_values.end() ? t[1] = _textures[it->second] : NULL;
			context->PSSetShaderResources(0, 1 + (t[1] != NULL ? 1 : 0), t);
			mesh->render(context);
		} else {
			mesh->render(context);
			LOG_WARNING_LN_ONESHOT("Unable to find material: %s", mesh->_material_name.c_str());
		}
	}

  return true;
}

bool TestEffect3::load_mesh(const string2& filename)
{
	container_delete(_meshes);

  ObjLoader loader;
  bool res = loader.load_from_file(filename, &_meshes);
	if (res) {
		float r = _meshes[0]->_bounding_radius;
		float fov = deg_to_rad(45) / (4/3.0f);
		float x = atanf(fov);
		float a = (r - r * x) / x;
		_cam_radius = r + a;
	}


	return res;
}

void TestEffect3::effect_loaded(EffectWrapper *effect)
{
  SAFE_DELETE(_effect);
  _effect = effect;
	for (int i = 0; i < (int)_meshes.size(); ++i)
		_meshes[i]->set_layout(_effect->create_input_layout(_meshes[i]->input_desc()));
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
	_cam_radius -= info.wheel_delta;		
}

bool TestEffect3::load_material(const string2& material_name)
{
	map_delete(_materials);
	_textures.clear();

	ObjLoader loader;
	ObjLoader::Materials materials;
	auto& s = System::instance();
	auto d = Graphics::instance().device();
	loader.load_material_file(material_name, &materials);

	for (int i = 0; i < (int)materials.size(); ++i) {
		_materials.insert(std::make_pair(materials[i]->name, materials[i]));
	}

	string2 map_names[] = { "map_Ka", "map_Kd", "map_d", "map_bump", "bump" };

	// load textures
	for (auto i = materials.begin(); i != materials.end(); ++i) {
		for (auto j = (*i)->string_values.begin(); j != (*i)->string_values.end(); ++j) {
			const string2& name = j->first;

			for (int k = 0; k < ELEMS_IN_ARRAY(map_names); ++k) {
				if (name == map_names[k]) {
					const string2& value = j->second;
					// check if we've already loaded the texture
					if (_textures.find(value) == _textures.end()) {

						Path p(value);
						string2 filename = Path::replace_extension(p.get_filename(), "bmp");
						ID3D11ShaderResourceView *t;
						string2 f = s.convert_path("data/textures/sponza_textures/textures/" + filename, System::kDirDropBox);
						if (file_exists(f)) {
							RETURN_ON_FAIL_BOOL_E(D3DX11CreateShaderResourceViewFromFile(d, f, NULL, NULL, &t, NULL));
							_textures.insert(std::make_pair(value, CComPtr<ID3D11ShaderResourceView>(t)));
						} else {
							LOG_WARNING_LN_ONESHOT("Unable to load texture: %s", f.c_str());
							_textures.insert(std::make_pair(value, CComPtr<ID3D11ShaderResourceView>(0)));
						}
					}

				}
			}
		}
	}

	return true;
}
