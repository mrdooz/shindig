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
  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);
};

