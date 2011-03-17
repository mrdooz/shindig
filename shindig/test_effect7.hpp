#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "app.hpp"

class Mesh2;
struct Material;

class TestEffect7 : public EffectBase
{
public:
  TestEffect7();
  virtual ~TestEffect7();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:

  void update(float t, float dt, int num_ticks, float a);

  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);

  DynamicVb<PosNormal> _verts;
  CComPtr<ID3D11InputLayout> _layout;
  std::auto_ptr<EffectWrapper> _effect;
  int _vertex_count;
};

