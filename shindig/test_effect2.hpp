#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "dynamic_vb.hpp"

struct PosCol { D3DXVECTOR3 p; D3DXCOLOR c; };

class TestEffect2 : public EffectBase
{
public:
  TestEffect2();
  virtual ~TestEffect2();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:

  struct Rect
  {
    Rect() {}
    Rect(const D3DXVECTOR3& c, const D3DXVECTOR3& e, const D3DXVECTOR3& r) : center(c), extents(e), rotation(r), x(1,0,0), y(0,1,0) {}
    PosCol *add_to_list(PosCol *ptr);
    D3DXVECTOR3	center;
    D3DXVECTOR3	extents;
    D3DXVECTOR3	rotation;

    D3DXVECTOR3 x, y;   // frame
  };


  void render_background();
  void render_lines();

  void line_loaded(EffectWrapper *effect);
	void bg_loaded(EffectWrapper *effect);
	void particle_loaded(EffectWrapper *effect);

  bool init_bg(const string2& filename);
  bool init_lines(const string2& filename);

  PosCol *draw_debug_lines(PosCol *ptr);

  void make_pyth_tree(int levels, const Rect& start, std::vector<Rect> *out);
  void make_pyth_tree_inner(int cur_level, int max_level, float angle, const Rect& parent, std::vector<Rect> *out);

  CComPtr<ID3D11DepthStencilState> _default_dss;

  EffectWrapper *_background;
  CComPtr<ID3D11InputLayout> _bg_layout;
  CComPtr<ID3D11Buffer> _bg_vb;

  EffectWrapper *_line_effect;
  CComPtr<ID3D11InputLayout> _line_layout;
	DynamicVb<PosCol> _line_vb;
	CComPtr<ID3D11DepthStencilState> _line_dss;

	EffectWrapper *_particle_effect;
	CComPtr<ID3D11InputLayout> _particle_layout;

	struct ParticleVb
	{
		D3DXVECTOR3 pos;
		float scale;
	};
  DynamicVb<ParticleVb> _particle_vb;

  int _num_splits;
  std::vector<D3DXVECTOR3> _control_points;

  struct DebugLine
  {
    DebugLine() {}
    DebugLine(const D3DXVECTOR3& s, const D3DXVECTOR3& e, const D3DXCOLOR& c) : s(s), e(e), c(c) {}
    D3DXVECTOR3 s, e;
    D3DXCOLOR c;
  };

  std::vector<DebugLine> _debug_lines;
	CComPtr<ID3D11DepthStencilState> _particle_dss;
	CComPtr<ID3D11SamplerState> _sampler_state;
	CComPtr<ID3D11ShaderResourceView> _texture;
	CComPtr<ID3D11BlendState> _blend_state;
};

