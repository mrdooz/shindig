#ifndef _TEST_EFFECT_HPP_
#define _TEST_EFFECT_HPP_

#include "resource_manager.hpp"
#include "effect_wrapper.hpp"

struct TestEffect
{
  TestEffect();

	bool init();
	bool close();
	bool render();

	void states_loaded(const ResourceManager::BlendStates& states);
	void vs_loaded(EffectWrapper* effect);

	CComPtr<ID3D11BlendState> _blend_state;
	EffectWrapper* _effect;
};

#endif
