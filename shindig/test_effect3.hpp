#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "dynamic_vb.hpp"
#include "app.hpp"

class Mesh2;

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

  bool load_mesh(const std::string& filename);
  void effect_loaded(EffectWrapper *effect);

  float _alpha;
  float _theta;
	float _cam_radius;
  bool _first_update;
  MouseInfo _prev_mouse;

  CComPtr<ID3D11DepthStencilState> _dss;
  CComPtr<ID3D11BlendState> _blend_state;

  Mesh2 *_mesh;
  EffectWrapper *_effect;
};

