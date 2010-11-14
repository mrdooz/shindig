#include "stdafx.h"
#include "test_effect4.hpp"
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

// test to make sure my texture coords are pixel correct

TestEffect4::TestEffect4()
{
}

TestEffect4::~TestEffect4()
{
}


bool TestEffect4::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();

 
  float v = 0.5f;
  g.set_clear_color(D3DXCOLOR(v, v, v, 1));

  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/test_effect4.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
    MakeDelegate(this, &TestEffect4::effect_loaded)));

	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("effects/test_effect4_states.lua", System::kDirRelative), MakeDelegate(this, &TestEffect4::load_states), true));

  string2 f = s.convert_path("data/textures/quad.png", System::kDirDropBox);
  if (file_exists(f))
    RETURN_ON_FAIL_BOOL_E(D3DX11CreateShaderResourceViewFromFile(d, f, NULL, NULL, &_texture.p, NULL));

  RETURN_ON_FAIL_BOOL_E(_verts.create(10000));

  return true;
}

bool TestEffect4::close()
{
  return true;
}

PosTex *add_quad(PosTex *v, const PosTex& v0, const PosTex& v1, const PosTex& v2, const PosTex& v3)
{
  // 0, 1
  // 2, 3

  // 0, 1, 2
  // 2, 1, 3

  *v++ = v0;
  *v++ = v1;
  *v++ = v2;

  *v++ = v2;
  *v++ = v1;
  *v++ = v3;

  return v;
}

bool TestEffect4::render()
{
  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  // 0, 1a/b, 2
  // 3, 4a/b, 5

  // 0, 1
  // 2, 3


  float ofs = 1 / 4.0f; // 1 texel offset
  static PosTex v0(-1, +1, +0, +0, +0);
  static PosTex v1a(0, 1, 0, 0.5f - ofs, 0);
  static PosTex v1b(0, 1, 0, 0.5f, 0);
  static PosTex v2(1, 1, 0, 1 - ofs, 0);

  static PosTex v3(-1, -1, +0, +0, 1);
  static PosTex v4a(0, -1, 0, 0.5f - 1 / 4.0f, 1);
  static PosTex v4b(0, -1, 0, 0.5f, 1);
  static PosTex v5(1, -1, 0, 1, 1);


  PosTex *v = _verts.map();
  PosTex *org = v;
  v = add_quad(v, v0, v1a, v3, v4a);
  v = add_quad(v, v1b, v2, v4b, v5);

  const int num_verts = _verts.unmap(v);

  float blend_factors[] = {1, 1, 1, 1};
  context->OMSetDepthStencilState(_dss, 0);

	float blend_factor[] = { 1, 1, 1, 1 };
	context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);

	_effect->set_shaders(context);

	ID3D11SamplerState *samplers[] = { _sampler_state };
	context->PSSetSamplers(0, 1, samplers);

  ID3D11ShaderResourceView* t[] = { _texture };
  context->PSSetShaderResources(0, 1, t);
  
  context->IASetInputLayout(_layout);
  set_vb(context, _verts.get(), Verts::stride);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->Draw(num_verts, 0);

  return true;
}

bool TestEffect4::load_states(const string2& filename)
{
	auto& s = System::instance();
	if (!lua_load_states(filename, "default_blend", "default_dss", "default_sampler", NULL, &_blend_state.p, &_dss.p, &_sampler_state.p, NULL))
		return false;

	return true;
}

void TestEffect4::effect_loaded(EffectWrapper *effect)
{
	_effect.reset(effect);
  InputDesc(). 
    add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
    add("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0).
    create(_layout, _effect.get());
}

