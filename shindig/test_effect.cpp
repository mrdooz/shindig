#include "stdafx.h"
#include "test_effect.hpp"

TestEffect::TestEffect()
  : _effect(NULL)
{

}
bool TestEffect::init()
{
	ResourceManager& r = ResourceManager::instance();
	r.load_effect_states("/projects/shindig/effects/states.fx", fastdelegate::bind(&TestEffect::states_loaded, this, _1));
	r.load_vertex_shader("/projects/shindig/effects/SystemVS.fx", "vsMain", fastdelegate::bind(&TestEffect::vs_loaded, this, _1));
	return true;
}

bool TestEffect::close()
{
  SAFE_DELETE(_effect);
	_blend_state.Release();
	return true;
}

bool TestEffect::render()
{
	return true;
}

void TestEffect::states_loaded(const ResourceManager::BlendStates& states)
{
	_blend_state.Release();
	_blend_state = states.find("AdditiveBlending")->second;
}

void TestEffect::vs_loaded(EffectWrapper* effect)
{
  SAFE_DELETE(effect);
  _effect = effect;
}
