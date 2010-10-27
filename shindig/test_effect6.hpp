#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "app.hpp"

class Mesh2;
struct Material;
struct Bezier;

class TestEffect6 : public EffectBase
{
public:
  TestEffect6();
  virtual ~TestEffect6();

	virtual bool init();
	virtual bool close();
  virtual bool update(float t, float dt, int num_ticks, float a);
  virtual bool post_update(float t, float dt, float a);
	virtual bool render();

private:

  void extrude(const Bezier& b);

  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);

  DynamicVb<D3DXVECTOR3> _verts;
  CComPtr<ID3D11InputLayout> _layout;
  std::auto_ptr<EffectWrapper> _effect;
  int _vertex_count;

};

