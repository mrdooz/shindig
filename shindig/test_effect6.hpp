#pragma once

#include "resource_manager.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "effect_base.hpp"
#include "app.hpp"

class Mesh2;
struct Material;

struct Bezier
{
  static Bezier from_points(AsArray<D3DXVECTOR3> data_points);
  D3DXVECTOR3 interpolate(float t) const;
  D3DXVECTOR3 diff1(float t) const;
  D3DXVECTOR3 diff2(float t) const;

  struct ControlPoints
  {
    ControlPoints(const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3) : p0(p0), p1(p1), p2(p2), p3(p3) {}
    D3DXVECTOR3 p0, p1, p2, p3;
  };

  std::vector<ControlPoints> curves;
};


class TestEffect6 : public EffectBase
{
public:
  TestEffect6();
  virtual ~TestEffect6();

	virtual bool init();
	virtual bool close();
	virtual bool render();

private:

  void update(float t, float dt, int num_ticks, float a);

  void extrude(const Bezier& b);
  void extrude2(const Bezier& b);

  void effect_loaded(EffectWrapper *effect);
	bool load_states(const string2& filename);

  Bezier _full_curve;

  std::vector<D3DXVECTOR3> _cur_points;
  float _progress;
  AppendableVb<PosNormal> _verts;
  DynamicVb<PosNormal> _tmp_verts;
  CComPtr<ID3D11InputLayout> _layout;
  std::auto_ptr<EffectWrapper> _effect;
  int _vertex_count;

  D3DXVECTOR3 _up;
  D3DXVECTOR3 _cur_top;
  float _last_update;
  float _angle;
};

