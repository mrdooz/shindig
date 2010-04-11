#include "stdafx.h"
#include "test_effect.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"

TestEffect::TestEffect()
  : _effect(NULL)
{
	container_delete(_scene._meshes);
}

bool TestEffect::init()
{
  System& sys = System::instance();
	ResourceManager& r = ResourceManager::instance();

  r.load_effect_states(sys.convert_path("effects/states.fx", System::kDirRelative).c_str(), fastdelegate::MakeDelegate(this, &TestEffect::states_loaded));
	r.load_vertex_shader(sys.convert_path("effects/SystemVS.fx", System::kDirRelative).c_str(), "vsMain", fastdelegate::MakeDelegate(this, &TestEffect::vs_loaded));

  ReduxLoader loader(sys.convert_path("data/scenes/diskette.rdx", System::kDirDropBox), &_scene, NULL);
	loader.load();
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
