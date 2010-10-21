#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "app.hpp"

class Mesh2;
struct Material;

struct Vehicle;

class TestEffect5 : public EffectBase
{
public:
  TestEffect5();
  virtual ~TestEffect5();

	virtual bool init();
	virtual bool close();
  virtual bool update(float t, float dt, int num_ticks, float a);
  virtual bool post_update(float t, float dt, float a);
	virtual bool render();

private:
  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);

  Vehicle *_vehicle;

  typedef std::vector<D3DXVECTOR3> Verts;
  Verts _verts;
};

