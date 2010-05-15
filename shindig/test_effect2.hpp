#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "dynamic_vb.hpp"

class TestEffect2 : public EffectBase
{
public:
  TestEffect2();
  virtual ~TestEffect2();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:

  void render_background();
  void render_lines();

  void line_loaded(EffectWrapper *effect);

  void bg_loaded(EffectWrapper *effect);
  bool init_bg(const std::string& filename);
  bool init_lines(const std::string& filename);

  CComPtr<ID3D11DepthStencilState> _default_dss;

  EffectWrapper *_background;
  CComPtr<ID3D11InputLayout> _bg_layout;
  CComPtr<ID3D11Buffer> _bg_vb;

  EffectWrapper *_line_effect;
  CComPtr<ID3D11InputLayout> _line_layout;
  DynamicVb<D3DXVECTOR3> _line_vb;
  CComPtr<ID3D11DepthStencilState> _line_dss;

  int _num_splits;
  std::vector<D3DXVECTOR3> _control_points;
};

