#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "dynamic_vb.hpp"

class TestEffect3 : public EffectBase
{
public:
  TestEffect3();
  virtual ~TestEffect3();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:

  bool load_mesh(const std::string& filename);
};

