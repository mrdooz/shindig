#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "dynamic_vb.hpp"
#include "app.hpp"

class Mesh2;
class Geometry;
struct Material;

class TestEffect3 : public EffectBase
{
public:
  TestEffect3();
  virtual ~TestEffect3();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:

  D3DXVECTOR3 calc_cam_pos() const;

	void on_mouse_move(const MouseInfo& info);
	void on_mouse_up(const MouseInfo& info);
	void on_mouse_down(const MouseInfo& info);
	void on_mouse_wheel(const MouseInfo& info);

	bool load_material(const string2& material_name);
  bool load_mesh(const string2& filename);
  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);

  float _alpha;
  float _theta;
	float _cam_radius;
  bool _first_update;
  MouseInfo _prev_mouse;

	std::map< string2, CComPtr<ID3D11ShaderResourceView> > _textures;

  CComPtr<ID3D11DepthStencilState> _dss;
  CComPtr<ID3D11BlendState> _blend_state;
	CComPtr<ID3D11SamplerState> _sampler_state;

	typedef std::map< string2, Material *> Materials;
	typedef std::vector<Geometry *> Geometries;
	Materials _materials;
	Geometries _geometries;
	std::auto_ptr<EffectWrapper> _effect;

	D3DXVECTOR3 _test;
};

