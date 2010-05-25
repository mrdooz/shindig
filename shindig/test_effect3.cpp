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
{
}

TestEffect3::~TestEffect3()
{
  SAFE_DELETE(_mesh);
}

bool TestEffect3::init()
{
  using namespace fastdelegate;
  using namespace rt;

  System& s = System::instance();

  s.add_file_changed(s.convert_path("sculptris/blob1.obj", System::kDirDropBox), MakeDelegate(this, &TestEffect3::load_mesh), true);

  return true;
}

bool TestEffect3::close()
{
  return true;
}


bool TestEffect3::render()
{
  return true;
}






bool TestEffect3::load_mesh(const std::string& filename)
{
  SAFE_DELETE(_mesh);

  ObjLoader loader;
  return loader.load_from_file(filename.c_str(), _mesh);

}
