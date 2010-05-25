#include "stdafx.h"
#include "test_effect3.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include "dynamic_vb.hpp"
#include <celsus/text_scanner.hpp>
#include <celsus/D3D11Descriptions.hpp>
#include "obj_loader.hpp"
#include "mesh2.hpp"

// wavefront obj loader



TestEffect3::TestEffect3()
  : _mesh(NULL)  
  , _effect(NULL)
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

  RETURN_ON_FAIL_BOOL(
    s.add_file_changed(s.convert_path("sculptris/blob1.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true),
    ErrorPredicate<bool>, LOG_ERROR_LN);

  RETURN_ON_FAIL_BOOL(r.load_shaders(s.convert_path("effects/sculptris_1.fx", System::kDirRelative).c_str(), "vsMain", "psMain", 
    MakeDelegate(this, &TestEffect3::effect_loaded)), ErrorPredicate<bool>, LOG_ERROR_LN);

  return true;
}

bool TestEffect3::close()
{
  return true;
}

float inline deg_to_rad(const float deg)
{
  return (float)D3DX_PI * deg / 180.0f;
}

bool TestEffect3::render()
{
  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  D3DXMATRIX view, proj;
  D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0,20, -20), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
  D3DXMatrixPerspectiveFovLH(&proj, deg_to_rad(45), 4 / 3.0f, 1, 100);
  _effect->set_vs_variable("mtx", view * proj);
  _effect->set_cbuffer();

  _effect->set_shaders(context);

  _mesh->render(context);

  return true;
}

bool TestEffect3::load_mesh(const std::string& filename)
{
  SAFE_DELETE(_mesh);

  ObjLoader loader;
  _mesh = new Mesh2();
  return loader.load_from_file(filename.c_str(), _mesh);
}

void TestEffect3::effect_loaded(EffectWrapper *effect)
{
  SAFE_DELETE(_effect);
  _effect = effect;

  D3D11_INPUT_ELEMENT_DESC desc[] = { 
    CD3D11_INPUT_ELEMENT_DESC("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0),
    CD3D11_INPUT_ELEMENT_DESC("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0),
  };
  _mesh->_layout = NULL;
  _mesh->_layout.Attach(_effect->create_input_layout(desc, ELEMS_IN_ARRAY(desc)));
}
