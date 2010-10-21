#include "stdafx.h"
#include "test_effect3.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include <celsus/text_scanner.hpp>
#include <celsus/file_utils.hpp>
#include <celsus/D3D11Descriptions.hpp>
#include <celsus/math_utils.hpp>
#include "obj_loader.hpp"
#include "mesh2.hpp"
#include "app.hpp"
#include "material.hpp"
#include "debug_menu.hpp"
#include "lua_utils.hpp"
#include "imgui.hpp"
#include "geometry.hpp"
#include "camera.hpp"

// wavefront obj viewer
TestEffect3::TestEffect3()
	: _cam_radius(150)
  , _alpha(0)
  , _theta(0)
  , _first_update(true)
	, _test(1.2f, 2.3f, 4.5f)
	, _camera(new FreeFlyCamera())
{
}

TestEffect3::~TestEffect3()
{
	using namespace fastdelegate;

	container_delete(_geometries);
}


bool TestEffect3::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();
  
  float v = 0; //0.5f;
  g.set_clear_color(D3DXCOLOR(v, v, v, 1));

#if 1
	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scenes/sponza_obj/sponza.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true));
	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scenes/sponza_obj/sponza.mtl", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_material), true));
	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/sponza.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &TestEffect3::effect_loaded)));
#else
	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("sculptris/blob1.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true));
	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/sculptris_1.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &TestEffect3::effect_loaded)));
#endif

	//RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("C:/Users/dooz/Downloads/PC-03_ON_OFF_Redux (1)/PC-03_Redux/Assets/Models/Scene1_Intro_Cube.obj", System::kDirAbsolute), 
		//MakeDelegate(this, &TestEffect3::load_mesh), true));

	

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/1.lua", System::kDirRelative), MakeDelegate(this, &TestEffect3::load_states), true));

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
	_camera->tick();

  if (_geometries.empty())
    return true;

  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  float blend_factors[] = {1, 1, 1, 1};
  context->OMSetDepthStencilState(_dss, 0);

	float blend_factor[] = { 1, 1, 1, 1 };
	context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);

  D3DXMATRIX view = _camera->view(), proj = _camera->proj();

	//D3DXMatrixLookAtLH(&view, &(_geometries[0]->bounding_sphere().center + calc_cam_pos()), &_geometries[0]->bounding_sphere().center, &D3DXVECTOR3(0,1,0));
	//D3DXMatrixPerspectiveFovLH(&proj, deg_to_rad(45), 4 / 3.0f, 1, 5000 /*_meshes[0]->_bounding_radius*/);
	D3DXMATRIX mtx;
	D3DXMatrixTranspose(&mtx, &(view * proj));
	_effect->set_vs_variable("mtx", mtx);
	_effect->set_cbuffer();
	_effect->set_shaders(context);

	ID3D11SamplerState *samplers[] = { _sampler_state };
	context->PSSetSamplers(0, 1, samplers);

	ID3D11ShaderResourceView* t[] = { 0, 0 };

	for (int i = 0; i < (int)_geometries.size(); ++i) {
		Mesh2 *mesh = _geometries[i]->mesh();
		if (Material *material = _materials[mesh->material_name()]) {
			t[0] = _textures[material->string_values["map_Kd"]];
			Material::StringValues::iterator it = material->string_values.find("map_d");
			t[1] = it != material->string_values.end() ? t[1] = _textures[it->second] : NULL;
			context->PSSetShaderResources(0, 1 + (t[1] != NULL ? 1 : 0), t);
			mesh->render(context);
		} else {
			mesh->render(context);
			LOG_WARNING_LN_ONESHOT("Unable to find material: %s", mesh->material_name().c_str());
		}
	}

  return true;
}

bool TestEffect3::load_mesh(const string2& filename)
{
	container_delete(_geometries);

  ObjLoader loader;
  bool res = loader.load_from_file(filename, &_geometries);
	if (res) {
		float r = _geometries[0]->bounding_sphere().radius;
		float fov = deg_to_rad(45) / (4/3.0f);
		float x = atanf(fov);
		float a = (r - r * x) / x;
		_cam_radius = r + a;
	}


	return res;
}

bool TestEffect3::load_states(const string2& filename)
{
	auto& s = System::instance();
	if (!lua_load_states(filename, "default_blend", "default_dss", "default_sampler", &_blend_state.p, &_dss.p, &_sampler_state.p))
		return false;

	return true;
}

void TestEffect3::effect_loaded(EffectWrapper *effect)
{
	if (!effect)
		return;

	_effect.reset(effect);
	for (int i = 0; i < (int)_geometries.size(); ++i)
		_geometries[i]->mesh()->set_layout(_effect.get()->create_input_layout(_geometries[i]->mesh()->input_desc()));
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
