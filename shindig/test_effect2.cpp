#include "stdafx.h"
#include "test_effect2.hpp"
#include "redux_loader.hpp"
#include "system.hpp"
#include "scene.hpp"
#include "mesh.hpp"

// Something like http://vimeo.com/1593564

namespace 
{
  const float width = 800;
  const float height = 600;
  const float far_plane = 1000;
  const float fov = static_cast<float>(D3DX_PI) * 0.25f;
  const float aspect_ratio = width / height;

  // Find a vector orthogonal to r, according to rtr (eq. 4.24, pg 71)
  D3DXVECTOR3 find_orthogonal(const D3DXVECTOR3& r)
  {
    const float abs_x = fabs(r.x);
    const float abs_y = fabs(r.y);
    const float abs_z = fabs(r.z);

    if (abs_x < abs_y && abs_x < abs_z) {
      return D3DXVECTOR3(0, -r.z, r.y);
    } else if (abs_y < abs_x && abs_y < abs_z) {
      return D3DXVECTOR3(-r.z, 0, r.x);
    } else {
      return D3DXVECTOR3(-r.y, r.x, 0);
    }
  }

  D3DXMATRIX matrix_from_vectors(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3, const D3DXVECTOR3& v4)
  {
    return D3DXMATRIX(
      v1.x, v1.y, v1.z, 0,
      v2.x, v2.y, v2.z, 0,
      v3.x, v3.y, v3.z, 0,
      v4.x, v4.y, v4.z, 1);
  }

  void calc_catmull_coeffs(D3DXVECTOR3& a, D3DXVECTOR3& b, D3DXVECTOR3& c, D3DXVECTOR3& d,
    const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
  {
    a = 0.5f * (-p0 + 3 * p1 - 3 * p2 + p3);
    b = 0.5f * (2 * p0 - 5 * p1 + 4 * p2 - p3);
    c = 0.5f * (-p0 + p2);
    d = 0.5f * (2 * p1);
  }

  D3DXVECTOR3 catmull(const float t, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
  {
    const float t2 = t * t;
    const float t3 = t * t2;
    D3DXVECTOR3 a, b, c, d;
    calc_catmull_coeffs(a, b, c, d, p0, p1, p2, p3);
    return t3 * a + t2 * b + t * c + d;
  }

  D3DXVECTOR3 catmull_vel(const float t, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
  {
    const float t2 = t * t;
    const float t3 = t * t2;
    D3DXVECTOR3 a, b, c, d;
    calc_catmull_coeffs(a, b, c, d, p0, p1, p2, p3);
    return 3 * t2 * a + 2 * t * b + c;
  }

  D3DXVECTOR3 catmull_acc(const float t, const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
  {
    const float t2 = t * t;
    const float t3 = t * t2;
    D3DXVECTOR3 a, b, c, d;
    calc_catmull_coeffs(a, b, c, d, p0, p1, p2, p3);
    return 6 * t * a + 2 * b;
  }

  void catmul_pos_vel(D3DXVECTOR3& pos, D3DXVECTOR3& vel, const float t, 
    const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3)
  {
    const float t2 = t * t;
    const float t3 = t * t2;
    D3DXVECTOR3 a, b, c, d;
    calc_catmull_coeffs(a, b, c, d, p0, p1, p2, p3);
    pos = t3 * a + t2 * b + t * c + d;
    // vel = pos'
    vel = 3 * t2 * a + 2 * t * b + c;
  }

}

#if 0
namespace mpl = boost::mpl;


struct Tentacle
{
  D3DXVECTOR3 prev_tangent;

  void init()
  {
    splits_ = 20;
    start_time_ = 0;

    control_points_.push_back(D3DXVECTOR3(0,0,0));
    control_points_.push_back(D3DXVECTOR3(0,5,0));
  }

  void render(const uint32_t time)
  {
    if (start_time_ == 0) {
      start_time_ = time;
    }

    const uint32_t growth_rate = 100;

    if ((time - start_time_) / growth_rate > control_points_.size()) {
      const float angle = randf((float)-D3DX_PI/6, (float)D3DX_PI/6);
      D3DXVECTOR3 tmp_tangent(prev_tangent);
      D3DXVec3Normalize(&tmp_tangent, &tmp_tangent);
      //D3DXVECTOR3 new_point(cosf(angle)/tmp_tangent);
      D3DXVECTOR3 new_point(0,0,0);

      control_points_.push_back(control_points_.back() + new_point);
    }

    vb->start_frame();

    float inc = 1 / (float)(splits_ - 1);

    const uint32_t spins = 20;

    bool first_point = true;

    D3DXVECTOR3 binormal;
    D3DXVECTOR3 prev_binormal;
    D3DXVECTOR3 tangent;
    D3DXVECTOR3 normal;
    D3DXVECTOR3 prev_normal;

    const int32_t num_knots = control_points_.size();
    for (int32_t i = 0; i < num_knots-1; ++i) {
      const bool last_knot = i == num_knots - 2;
      const D3DXVECTOR3 p_1 = control_points_[max(0,i-1)];
      const D3DXVECTOR3 p0 = control_points_[i+0];
      const D3DXVECTOR3 p1 = control_points_[min(num_knots-1, i+1)];
      const D3DXVECTOR3 p2 = control_points_[min(num_knots-1, i+2)];

      D3DXVECTOR3 m0 = (p1 - p_1) / 2;
      D3DXVECTOR3 m1 = (p2 - p0) / 2;

      float cur_t = 0;
      for (uint32_t j = 0; j < splits_; ++j, cur_t += inc) {
        D3DXVECTOR3 cur;
        D3DXVec3Hermite(&cur, &p0, &m0, &p1, &m1, cur_t);
        cur = catmull(cur_t, p_1, p0, p1, p2);
        tangent = catmull_vel(cur_t, p_1, p0, p1, p2);
        D3DXVec3Normalize(&tangent, &tangent);

        if (first_point) {
          D3DXVECTOR3 q = catmull_acc(cur_t, p_1, p0, p1, p2);
          D3DXVec3Cross(&normal, &tangent, &q);
          if (D3DXVec3LengthSq(&normal) == 0) {
            // if the curvature is 0, choose any normal perpedictular to the tangent
            normal = find_orthogonal(tangent);
          } else {
            D3DXVec3Cross(&normal, &normal, &tangent);
          }
          D3DXVec3Normalize(&normal, &normal);

          D3DXVec3Cross(&binormal, &tangent, &normal);
          first_point = false;
        } else {
          // Ken Sloan's method to propagate the reference frame
          D3DXVec3Cross(&normal, &prev_binormal, &tangent);
          D3DXVec3Cross(&binormal, &tangent, &normal);
        }

        prev_binormal = binormal;
        prev_tangent = tangent;


        const float delta_angle = 2 * (float)D3DX_PI / (float)(spins - 1);
        float cur_angle = 0;
        D3DXVECTOR3 tmp;
        D3DXVECTOR3 tmp2;
        D3DXMATRIX mtx_rot2(kMtxId);

        D3DXVECTOR3 scaled_s = normal;
        if (last_knot) {
          scaled_s *= ((splits_-1) - j) / (float)splits_;
        }

        for (uint32_t k = 0; k < spins; ++k, cur_angle += delta_angle) {
          D3DXMatrixRotationAxis(&mtx_rot2, &tangent, cur_angle);
          D3DXVec3TransformCoord(&tmp, &scaled_s, &mtx_rot2);
          D3DXVec3TransformNormal(&tmp2, &normal, &mtx_rot2);

          vb->add(tmp + cur, tmp2);
        }
      }
    }

    ib->start_frame();
    for (uint32_t i = 0; i < vb->vertex_count() / spins  - 1; ++i) {
      for (uint32_t j = 0; j < spins; ++j) {

        // d-c
        // | |
        // b-a
        const uint32_t next_j = (j + 1) % spins;
        const uint32_t a = i*spins + j;
        const uint32_t b = i*spins + next_j;
        const uint32_t c = (i+1)*spins + j;
        const uint32_t d = (i+1)*spins + next_j;

        // a, b, c
        ib->add(a);
        ib->add(b);
        ib->add(c);

        // b, d, c
        ib->add(b);
        ib->add(d);
        ib->add(c);
      }
    }
    ib->end_frame();

    vb->end_frame();
    vb->set_input_layout();
    vb->set_vertex_buffer();
    ib->set_index_buffer();
    g_d3d_device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_d3d_device->DrawIndexed(ib->index_count(), 0, 0);
  }

  typedef std::vector<D3DXVECTOR3> ControlPoints;
  ControlPoints control_points_;
  uint32_t splits_;
  uint32_t start_time_;
};

Tentacle tentacle;
#endif

TestEffect2::TestEffect2()
  : _background(nullptr)
  , _line_effect(nullptr)
  , _num_splits(10)
  , _particle_effect(nullptr)
{
}

TestEffect2::~TestEffect2()
{
  SAFE_DELETE(_background);
  SAFE_DELETE(_line_effect);
  SAFE_DELETE(_particle_effect);
}

bool TestEffect2::init()
{
  using namespace fastdelegate;
  using namespace rt;

  System& s = System::instance();
  ResourceManager& r = ResourceManager::instance();
  Graphics& g = Graphics::instance();
  ID3D11Device *d = g.device();

  _default_dss.Attach(D3D11::DepthStencilDescription().Create(d));
  _line_dss.Attach(D3D11::DepthStencilDescription().DepthEnable_(FALSE).Create(d));

  // init bg
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/gradient_quad.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
    MakeDelegate(this, &TestEffect2::bg_loaded)));

	InputDesc().
		add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
		create(_bg_layout, _background);

  s.add_file_changed(s.convert_path("data/settings/testeffect2.txt", System::kDirRelative), MakeDelegate(this, &TestEffect2::init_bg), true);
  s.add_file_changed(s.convert_path("data/settings/testeffect2.txt", System::kDirRelative), MakeDelegate(this, &TestEffect2::init_lines), true);

  // init lines
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/single_color.fx", System::kDirRelative), "vsMain", NULL, "psMain", 
    MakeDelegate(this, &TestEffect2::line_loaded)));

  if (!_line_vb.create(10000))
    return false;

	InputDesc().
		add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
		create(_line_layout, _line_effect);

  // init particles
	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/particle.fx", System::kDirRelative), "vsMain", "gsMain", "psMain", 
		MakeDelegate(this, &TestEffect2::particle_loaded)));

  RETURN_ON_FAIL_BOOL_E(_particle_vb.create(10000));
  RETURN_ON_FAIL_BOOL_E(InputDesc().
		add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("TEXCOORD", 0, DXGI_FORMAT_R32_FLOAT, 0).
		create(_particle_layout, _particle_effect));

	_sampler_state.Attach(D3D11::SamplerDescription().
		AddressU_(D3D11_TEXTURE_ADDRESS_CLAMP).
		AddressV_(D3D11_TEXTURE_ADDRESS_CLAMP).
		Filter_(D3D11_FILTER_MIN_MAG_MIP_LINEAR).
		Create(d));

	_blend_state.Attach(D3D11::BlendDescription().
		RenderTarget_(0, D3D11::RenderTargetBlendDescription().
			BlendEnable_(TRUE).
			BlendOp_(D3D11_BLEND_OP_ADD).
			BlendOpAlpha_(D3D11_BLEND_OP_ADD).
			SrcBlend_(D3D11_BLEND_ONE).
			DestBlend_(D3D11_BLEND_ONE).
			SrcBlendAlpha_(D3D11_BLEND_SRC_ALPHA).
			DestBlendAlpha_(D3D11_BLEND_INV_SRC_ALPHA)).
		Create(d));

	_particle_dss.Attach(D3D11::DepthStencilDescription().
		DepthEnable_(FALSE).
		Create(d));
		

	RETURN_ON_FAIL_BOOL_E(D3DX11CreateShaderResourceViewFromFile(d, s.convert_path("data/gfx/particle.png", System::kDirDropBox),
		NULL, NULL, &_texture, NULL));

  return true;
}

bool TestEffect2::init_bg(const string2& filename)
{
  Graphics& g = Graphics::instance();
  SectionReader r;
  if (!r.load(filename, "background"))
    return false;

  // clip space triangle strip
  // 1 3
  // 0 2
  struct 
  {
    D3DXVECTOR3 pos;
    D3DXCOLOR col;
  } verts[4];

  for (int i = 0; i < 4; ++i) {
    if (!r.read_vector3(&verts[i].pos)) 
      return false;
    if (!r.read_color(&verts[i].col))
      return false;
  }

  _bg_vb = NULL;
  return SUCCEEDED(create_static_vertex_buffer(g.device(), 4, sizeof(verts[0]), (const uint8_t*)&verts, &_bg_vb));
}


bool TestEffect2::close()
{
  return true;
}


bool TestEffect2::render()
{
  render_background();
  render_lines();
  return true;
}

void TestEffect2::render_background()
{
  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  context->OMSetDepthStencilState(_default_dss, 0);

  _background->set_shaders(context);
  context->IASetInputLayout(_bg_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  set_vb(context, _bg_vb, sizeof(D3DXVECTOR3) + sizeof(D3DXCOLOR));
  context->Draw(4, 0);

}


PosCol *TestEffect2::Rect::add_to_list(PosCol *ptr)
{
	D3DXMATRIX rot;
	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, center.x, center.y, center.z);
	D3DXMatrixRotationZ(&rot, rotation.z);

	D3DXMATRIX mtx = rot * trans;

	// 0-1
	// 2-3

	D3DXVECTOR3 v0(-extents.x, +extents.y, +extents.z);
	D3DXVECTOR3 v1(+extents.x, +extents.y, +extents.z);
	D3DXVECTOR3 v2(-extents.x, -extents.y, +extents.z);
	D3DXVECTOR3 v3(+extents.x, -extents.y, +extents.z);

	D3DXVec3TransformCoord(&ptr->pos, &v0, &mtx); ptr++;
	D3DXVec3TransformCoord(&ptr->pos, &v1, &mtx); ptr++;

	D3DXVec3TransformCoord(&ptr->pos, &v1, &mtx); ptr++;
	D3DXVec3TransformCoord(&ptr->pos, &v3, &mtx); ptr++;

	D3DXVec3TransformCoord(&ptr->pos, &v3, &mtx); ptr++;
	D3DXVec3TransformCoord(&ptr->pos, &v2, &mtx); ptr++;

	D3DXVec3TransformCoord(&ptr->pos, &v2, &mtx); ptr++;
	D3DXVec3TransformCoord(&ptr->pos, &v0, &mtx); ptr++;

	return ptr;
}

void TestEffect2::make_pyth_tree_inner(int cur_level, int max_level, float angle, const Rect& parent, std::vector<Rect> *out)
{

	if (cur_level > max_level)
		return;

  float b = 0;
	// calc new basis vectors
  {
    D3DXMATRIX r;
    Rect left;
    D3DXMatrixRotationZ(&r, angle);
    D3DXVec3TransformCoord(&left.x, &parent.x, &r);
    D3DXVec3TransformCoord(&left.y, &parent.y, &r);

    b = parent.extents.x * cosf(angle);

    D3DXVECTOR3 pivot(-parent.extents.x, parent.extents.y, 0);
    D3DXMATRIX rp;
    D3DXMatrixRotationZ(&rp, parent.rotation.z);
    D3DXVec3TransformCoord(&pivot, &pivot, &rp);

    left.center = b * (left.x + left.y) + parent.center + pivot;
    left.extents = b * D3DXVECTOR3(1,1,1);
    left.rotation = D3DXVECTOR3(0, 0, parent.rotation.z + angle);
    out->push_back(left);

    _debug_lines.push_back(DebugLine(left.center, left.center + b * left.x, D3DXCOLOR(1,0,0,1)));
    _debug_lines.push_back(DebugLine(left.center, left.center + b * left.y, D3DXCOLOR(0,1,0,1)));

    make_pyth_tree_inner(cur_level+1, max_level, angle, left, out);
  }

  {
    const float old_angle = angle;
    angle = -((float)D3DX_PI/2 - angle);
    D3DXMATRIX r;
    Rect left;
    D3DXMatrixRotationZ(&r, angle);
    D3DXVec3TransformCoord(&left.x, &parent.x, &r);
    D3DXVec3TransformCoord(&left.y, &parent.y, &r);

    float a = parent.extents.x;
    b = sqrtf(a*a - b*b);

    D3DXVECTOR3 pivot(parent.extents.x, parent.extents.y, 0);
    D3DXMATRIX rp;
    D3DXMatrixRotationZ(&rp, parent.rotation.z);
    D3DXVec3TransformCoord(&pivot, &pivot, &rp);

    left.center = -b * left.x + b * left.y + parent.center + pivot;
    left.extents = b * D3DXVECTOR3(1,1,1);
    left.rotation = D3DXVECTOR3(0, 0, parent.rotation.z + angle);
    out->push_back(left);

    _debug_lines.push_back(DebugLine(left.center, left.center + b * left.x, D3DXCOLOR(1,0,0,1)));
    _debug_lines.push_back(DebugLine(left.center, left.center + b * left.y, D3DXCOLOR(0,1,0,1)));

    make_pyth_tree_inner(cur_level+1, max_level, old_angle, left, out);
  }
}

static int hax = 0;

void TestEffect2::make_pyth_tree(int levels, const Rect& start, std::vector<Rect> *out)
{
	out->push_back(start);
	make_pyth_tree_inner(1, levels, sinf(hax++ / 1000.0f) * (float)D3DX_PI / 2, start, out);
	//make_pyth_tree_inner(1, levels, 0.5, start, out);
//	make_pyth_tree_inner(1, levels, 0, start, out);
}

PosCol *TestEffect2::draw_debug_lines(PosCol *ptr)
{
  for (int i = 0; i < (int)_debug_lines.size(); ++i) {
    ptr[0].pos = _debug_lines[i].s;
    ptr[0].col = _debug_lines[i].c;
    ptr[1].pos = _debug_lines[i].e;
    ptr[1].col = _debug_lines[i].c;
    ptr += 2;
  }
  return ptr;
}

void TestEffect2::render_lines()
{
  _debug_lines.clear();

  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  std::vector<Rect> rects;
	float s = 0.1f;
  make_pyth_tree(9, Rect(D3DXVECTOR3(0,-0.5f,0), D3DXVECTOR3(s, s, s), D3DXVECTOR3(0,0,0)), &rects);

/*
  context->OMSetDepthStencilState(_line_dss, 0);

  D3DXMATRIX mtx;
  D3DXMatrixIdentity(&mtx);
  _line_effect->set_vs_variable("mtx", mtx);
  _line_effect->set_cbuffer();

  _line_effect->set_shaders(context);
  context->IASetInputLayout(_line_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  
	PosCol* p = _line_vb.map();

	int count = rects.size() * 4 * 2;
	for (int i = 0; i < (int)rects.size(); ++i)
		p = rects[i].add_to_list(p);

  p = draw_debug_lines(p);
  count += _debug_lines.size() * 2;

#if 0
  const int count = _control_points.size();
  for (int i = 0; i < count; ++i) {
    D3DXVECTOR3 p0 = _control_points[std::max<int>(0, i-1)];
    D3DXVECTOR3 p1 = _control_points[std::max<int>(0, i-0)];
    D3DXVECTOR3 p2 = _control_points[std::min<int>(count-1, i+1)];
    D3DXVECTOR3 p3 = _control_points[std::min<int>(count-1, i+2)];

    for (int j = 0; j < _num_splits; ++j) {
      float t = j / (float)_num_splits;
      D3DXVECTOR3 v = catmull(t, p0, p1, p2, p3);
      *p++ = v;
    }
  }
#endif
  _line_vb.unmap();

  set_vb(context, _line_vb.vb(), sizeof(PosCol));
  //if (count > 0)
//		context->Draw(count, 0);
*/
  // draw the particles
  const int particle_count = (int)rects.size();
  if (particle_count > 0 ) {
    ParticleVb *particles = _particle_vb.map();
    for (int i = 0; i < particle_count; ++i) {
			particles->pos = rects[i].center;
			particles->scale = rects[i].extents.x;
			particles++;
    }
    _particle_vb.unmap();

    D3DXMATRIX mtx;
    D3DXMatrixIdentity(&mtx);
    _particle_effect->set_gs_variable("mtx", mtx);
    _particle_effect->set_cbuffer();

    _particle_effect->set_shaders(context);
    context->IASetInputLayout(_particle_layout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    set_vb(context, _particle_vb.vb(), sizeof(ParticleVb));

		ID3D11ShaderResourceView* t[] = { _texture };
		ID3D11SamplerState *samplers[] = { _sampler_state };
		float blend_factors[4] = {1, 1, 1, 1};
		context->OMSetBlendState(_blend_state, blend_factors, 0xffffffff);
		context->OMSetDepthStencilState(_particle_dss, 0xffffffff);
		context->PSSetSamplers(0, 1, samplers);
		context->PSSetShaderResources(0, 1, t);

    context->Draw(particle_count, 0);
  }

}

void TestEffect2::line_loaded(EffectWrapper *effect)
{
  delete exch(_line_effect, effect);
}

void TestEffect2::bg_loaded(EffectWrapper *effect)
{
  delete exch(_background, effect);
}

bool TestEffect2::init_lines(const string2& filename)
{
  SectionReader r;
  if (!r.load(filename, "control-points"))
    return false;

  if (!r.read_int(&_num_splits))
    return false;

  _control_points.clear();
  D3DXVECTOR3 tmp;
  while (r.read_vector3(&tmp))
    _control_points.push_back(tmp);

  return true;
}

void TestEffect2::particle_loaded(EffectWrapper *effect)
{
	delete exch(_particle_effect, effect);
}
