#include "stdafx.h"
#include "test_effect6.hpp"
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
#include "debug_renderer.hpp"

using namespace std;

struct Spline
{
  void add_point(const D3DXVECTOR3& pt);
  D3DXVECTOR3 interpolate(float t) const;
  vector<D3DXVECTOR3> _pts;
};

void Spline::add_point(const D3DXVECTOR3& pt)
{
  _pts.push_back(pt);
}

D3DXVECTOR3 Spline::interpolate(float t) const
{
  return D3DXVECTOR3(0,0,0);
}



TestEffect6::TestEffect6()
{
}

TestEffect6::~TestEffect6()
{
}

bool TestEffect6::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();

  return true;
}

bool TestEffect6::close()
{
  return true;
}

bool TestEffect6::render()
{
  Graphics& g = Graphics::instance();
  ID3D11Device* device = g.device();
  ID3D11DeviceContext* context = g.context();

  context->OMSetDepthStencilState(g.default_dss(), g.default_stencil_ref());
  context->OMSetBlendState(g.default_blend_state(), g.default_blend_factors(), g.default_sample_mask());

  
  return true;
}

bool TestEffect6::load_states(const string2& filename)
{
	return true;
}

void TestEffect6::effect_loaded(EffectWrapper *effect)
{
}

bool TestEffect6::update(float t, float dt, int num_ticks, float a)
{
  return true;   

}

bool TestEffect6::post_update(float t, float dt, float a)
{
  return true;
}
