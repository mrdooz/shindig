#include "stdafx.h"
#include "test_effect5.hpp"
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
#include "lua_utils.hpp"
#include "debug_renderer.hpp"

struct Cube
{
  void render();
  D3DXVECTOR3 scale;
  D3DXVECTOR3 center;
};

void Cube::render()
{
  D3DXMATRIX mtx;
  D3DXMatrixTransformation(&mtx, &kVec3Zero, &kQuatId, &scale, &kVec3Zero, &kQuatId, &center);

  //   2__3
  //  /  /
  // 0--1

  static D3DXVECTOR3 verts[] = 
  {
    D3DXVECTOR3(-1, -1, +1),
    D3DXVECTOR3(+1, -1, +1),
    D3DXVECTOR3(-1, -1, -1),
    D3DXVECTOR3(+1, -1, -1),
    D3DXVECTOR3(-1, +1, +1),
    D3DXVECTOR3(+1, +1, +1),
    D3DXVECTOR3(-1, +1, -1),
    D3DXVECTOR3(+1, +1, -1),
  };

  static int edges[] = 
  {
    0, 1,
    0, 2,
    1, 3,
    2, 3,
    4, 5,
    4, 6,
    5, 7,
    6, 7,
    0, 4,
    1, 5,
    2, 6,
    3, 7,
  };

  D3DXVECTOR3 v[ELEMS_IN_ARRAY(verts)];
  D3DXVec3TransformCoordArray(v, sizeof(D3DXVECTOR3), verts, sizeof(D3DXVECTOR3), &mtx, ELEMS_IN_ARRAY(verts));
  for (int i = 0; i < ELEMS_IN_ARRAY(edges)/2; ++i) {
    const int i0 = edges[i*2+0];
    const int i1 = edges[i*2+1];
    DebugRenderer::instance().add_line(v[i0], v[i1], D3DXCOLOR(1,1,1,1), kMtxId);
  }


}


// terminology from the steering behavior paper
struct Vehicle
{
  Vehicle() 
    : pos(0,0,0)
    , vel(0,0,0)
    , acc(0,0,0)
    , _angle(0)
  {
  }

  void integrate(float dt)
  {
    // velocity Verlet scheme
    pos += vel * dt + 0.5f * acc * dt * dt;
    D3DXVECTOR3 tmp = vel + 0.5f * acc * dt;
    vel = tmp + 0.5f * acc * dt;
  }

  float _angle;   // rotation counter-clockwise

  D3DXVECTOR3 pos;
  D3DXVECTOR3 pos_old;
  D3DXVECTOR3 acc;

  // transient
  D3DXVECTOR3 vel;
};


TestEffect5::TestEffect5()
  : _vehicle(new Vehicle())
{
}

TestEffect5::~TestEffect5()
{
  SAFE_DELETE(_vehicle);
}

bool TestEffect5::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();

  return true;
}

bool TestEffect5::close()
{
  return true;
}

bool TestEffect5::render()
{
  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  Cube c;
  c.center = kVec3Zero;
  c.scale = 5 * kVec3One;

  c.render();

  //for (int i = 0; i < (int)_verts.size() - 1; ++i) {
    //DebugRenderer::instance().add_line(_verts[i], _verts[i+1], D3DXCOLOR(1,1,1,1), kMtxId);
  //}

  return true;
}

bool TestEffect5::load_states(const string2& filename)
{
	return true;
}

void TestEffect5::effect_loaded(EffectWrapper *effect)
{
}

bool TestEffect5::update(float t, float dt, int num_ticks, float a)
{
  // add -gravity
  D3DXVECTOR3 g(0,1,0);
  float m = 1;
  D3DXVECTOR3 acc = g / m;
  _vehicle->acc = acc;
  for (int i = 0; i < num_ticks; ++i) {
    _vehicle->integrate(dt);
  }

  static int hax = 0;
  if ((++hax % 20) == 0) {
    _verts.push_back(_vehicle->pos);
  }

  // adjust angle on wander sphere
  _vehicle->_angle += deg_to_rad(randf(-10, 10));


  return true;   

}

bool TestEffect5::post_update(float t, float dt, float a)
{
  return true;
}
