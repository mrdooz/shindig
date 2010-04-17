#ifndef _TEST_EFFECT_HPP_
#define _TEST_EFFECT_HPP_

#include "resource_manager.hpp"
#include "effect_wrapper.hpp"
#include "scene.hpp"
#include "render_target.hpp"

struct TestEffect
{
  TestEffect();

	bool init();
	bool close();
	bool render();

	void states_loaded(const ResourceManager::BlendStates& states);
	void vs_loaded(EffectWrapper* effect);
	void ps_loaded(EffectWrapper* effect);
	void scene_loaded(Scene* scene);
	void materials_loaded(const MaterialFile& materials);

	RefPtr<Scene> _scene;
	MaterialFile _materials;
	EffectWrapper* _vs_effect;
	EffectWrapper* _ps_effect;
	CComPtr<ID3D11InputLayout> _layout;

	RenderTarget _rt;
	CComPtr<ID3D11RasterizerState> _rasterizer_state;
	CComPtr<ID3D11BlendState> _blend_state;
	CComPtr<ID3D11DepthStencilState> _depth_state;
};

#endif
