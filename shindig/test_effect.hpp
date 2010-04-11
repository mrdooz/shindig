#ifndef _TEST_EFFECT_HPP_
#define _TEST_EFFECT_HPP_

#include "resource_manager.hpp"
#include "effect_wrapper.hpp"
#include "scene.hpp"

struct TestEffect
{
  TestEffect();

	bool init();
	bool close();
	bool render();

	void states_loaded(const ResourceManager::BlendStates& states);
	void vs_loaded(EffectWrapper* effect);
	void ps_loaded(EffectWrapper* effect);

	Scene _scene;
	//CComPtr<ID3D11BlendState> _blend_state;
	EffectWrapper* _vs_effect;
	EffectWrapper* _ps_effect;
	ID3D11InputLayout* _layout;

	ID3D11RasterizerState* _rasterizer_state;
	ID3D11BlendState* _blend_state;
	ID3D11DepthStencilState* _depth_state;
};

#endif
