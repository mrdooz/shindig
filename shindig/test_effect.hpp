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

	void states_loaded(const ResourceManager::EffectStates& states);
	void post_vs_loaded(EffectWrapper* effect);
	void post_ps_loaded(EffectWrapper* effect);
	void vs_loaded(EffectWrapper* effect);
	void ps_loaded(EffectWrapper* effect);
	void scene_loaded(Scene* scene);
	void materials_loaded(const MaterialFile& materials);

	void	render_meshes();

	void	callback(const int idx);

	bool _boom;

	RefPtr<Scene> _scene;
	MaterialFile _materials;
	EffectWrapper* _vs_effect;
	EffectWrapper* _ps_effect;
	CComPtr<ID3D11InputLayout> _layout;

	CComPtr<ID3D11Buffer> _full_screen_vb;
	CComPtr<ID3D11Buffer> _full_screen_ib;
	CComPtr<ID3D11InputLayout> _full_screen_layout;
	D3D11_VIEWPORT _viewport;
	EffectWrapper* _vs_fs;
	EffectWrapper* _ps_fs;

	RenderTarget _rt;
	CComPtr<ID3D11RasterizerState> _rasterizer_state;
	CComPtr<ID3D11BlendState> _blend_state;
	CComPtr<ID3D11DepthStencilState> _depth_state;
	CComPtr<ID3D11SamplerState> _sampler_state;
};

#endif
