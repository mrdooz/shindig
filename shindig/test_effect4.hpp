#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "dynamic_vb.hpp"
#include "app.hpp"
#include <celsus/vertex_types.hpp>

class Mesh2;
struct Material;

class TestEffect4 : public EffectBase
{
public:
  TestEffect4();
  virtual ~TestEffect4();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:
  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);

  CComPtr<ID3D11DepthStencilState> _dss;
  CComPtr<ID3D11BlendState> _blend_state;
	CComPtr<ID3D11SamplerState> _sampler_state;
  CComPtr<ID3D11ShaderResourceView> _texture;
  CComPtr<ID3D11InputLayout> _layout;
  typedef DynamicVb<PosTex> Verts;
  Verts _verts;

  EffectWrapper *_effect;
};

