#include "stdafx.h"
#include "test_effect6.hpp"
#include "system.hpp"
#include "app.hpp"
#include "debug_menu.hpp"
#include "lua_utils.hpp"
#include "debug_renderer.hpp"
#include "camera.hpp"

using namespace std;

// Find a vector orthogonal to r, according to rtr (eq. 4.24, pg 71)
D3DXVECTOR3 find_orthogonal(const D3DXVECTOR3& r)
{
  const float abs_x = fabs(r.x);
  const float abs_y = fabs(r.y);
  const float abs_z = fabs(r.z);

  D3DXVECTOR3 res;

  if (abs_x < abs_y && abs_x < abs_z) {
    res = D3DXVECTOR3(0, -r.z, r.y);
  } else if (abs_y < abs_x && abs_y < abs_z) {
    res = D3DXVECTOR3(-r.z, 0, r.x);
  } else {
    res = D3DXVECTOR3(-r.y, r.x, 0);
  }
  return res;
}


template<typename T>
struct Matrix2d
{
  Matrix2d()
    : _data(NULL)
    , _rows(0)
    , _cols(0)
  {
  }

  Matrix2d(int rows, int cols) 
    : _rows(rows)
    , _cols(cols) 
  {
    _data = new T[rows * cols];
  }

  void init(int rows, int cols)
  {
    reset();
    _rows = rows;
    _cols = cols;
    _data = new T[rows*cols];
  }

  ~Matrix2d()
  {
    reset();
  }

  Matrix2d(const Matrix2d& rhs)
    : _data(NULL)
    , _rows(0)
    , _cols(0)
  {
    if (&rhs == this)
      return;

    assign(rhs);
  }

  Matrix2d& operator=(const Matrix2d& rhs)
  {
    reset();
    assign(rhs);
  }

  const T& at(int row, int col) const
  {
    return _data[row*_cols+col];
  }

  T& at(int row, int col)
  {
    return _data[row*_cols+col];
  }

  const T& operator()(int row, int col) const
  {
    return _data[row*_cols+col];
  }

  T& operator()(int row, int col)
  {
    return _data[row*_cols+col];
  }

  void reset()
  {
    delete [] _data;
    _data = NULL;
    _rows = _cols = 0;
  }

  void assign(const Matrix2d& rhs)
  {
    _rows = rhs._rows;
    _cols = rhs._cols;
    _data = new T[_rows * _cols];
  }

  int rows() const { return _rows; }
  int cols() const { return _cols; }

  void augment(const Matrix2d& a, Matrix2d *out)
  {
    // out = [this|a];
    out->init(rows(), cols() + a.cols());

    const int nr = out->rows();
    const int nc = out->cols();

    for (int i = 0; i < out->rows(); ++i) {
      memcpy(&out->_data[i*nc], &_data[i*cols()], cols()*sizeof(T));
      memcpy(&out->_data[i*nc+cols()], &a._data[i*a.cols()], a.cols()*sizeof(T));
    }
  }

  void console_print()
  {
    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < cols(); ++j) {
        console_printf("%8f ", at(i,j));
      }
      console_printf("\n");
    }
    console_printf("\n");
  }

  void print()
  {
    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < cols(); ++j) {
        printf("%8f ", at(i,j));
      }
      printf("\n");
    }
    printf("\n");
  }

  T* _data;
  int _rows;
  int _cols;
};


template<typename T>
void augment(const Matrix2d<T>& a, const Matrix2d<T>& b, Matrix2d<T> *out)
{
  // out = [a|b];
  out->init(a.rows(), a.cols() + b.cols());

  const int nr = out->rows();
  const int nc = out->cols();

  for (int i = 0; i < out->rows(); ++i) {
    memcpy(&out->_data[i*nc], &a._data[i*a.cols()], a.cols()*sizeof(T));
    memcpy(&out->_data[i*nc+a.cols()], &b._data[i*b.cols()], b.cols()*sizeof(T));
  }
}


template<typename T>
void gaussian_solve(Matrix2d<T>& c, Matrix2d<T> *x)
{
  // solve m*x = a via gaussian elimination
  // c is the augmented matrix, [m|a]

  x->init(c.rows(), 1);

  // TODO: do pivoting

  T eps = 0.00001f;

  // row reduction
  for (int i = 0; i < c.rows(); ++i) {
    const T v = c.at(i,i);
    // skip if we already have a leading 1
    if ((T)abs(1-v) < eps)
      continue;

    const T d = 1 / v;
    // make the leading element in the current row 1
    for (int j = i; j < c.cols(); ++j)
      c.at(i,j) = c.at(i,j) * d;

    // reduce the remaining rows to set 0s in the i:th column
    for (int j = i+1; j < c.rows(); ++j) {
      const T v = c.at(j,i);
      // skip if the element is 0 already
      if ((T)abs(v) < eps)
        continue;
      const T d = c.at(j,i) / c.at(i,i);
      for (int k = i; k < c.cols(); ++k)
        c.at(j,k) = c.at(j,k) - d * c.at(i,k);
    }
  }

  // backward substitution
  for (int i = 0; i < c.rows() - 1; ++i) {
    const int cur_row = c.rows() - 2 - i;
    const int start_col = cur_row + 1;
    for (int j = 0; j <= i; ++j) {
      const T s = c.at(cur_row, start_col+j);
      const T v = c.at(start_col+j, c.cols()-1);
      c.at(cur_row, c.cols()-1) = c.at(cur_row, c.cols()-1) - s * v;
      c.at(cur_row, start_col+j) = 0;
    }
  }

  // copy the solution
  for (int i = 0; i < c.rows(); ++i)
    x->at(i,0) = c.at(i,c.cols()-1);
}

template<typename T>
void gaussian_solve(const Matrix2d<T>& m, const Matrix2d<T>& a, Matrix2d<T> *x)
{
  // solve m*x = a via gaussian elimination
  assert(m.rows() == a.rows());
  assert(m.rows() == m.cols());

  // TODO: do pivoting

  // c = [m|a]
  Matrix2d<T> c;
  augment(m, a, &c);
  gaussian_solve(c, &x);
}

// wrapper around a <data,size> tuple
template<typename T>
class AsArray
{
public:
  AsArray(T* data, int n) : _data(data), _n(n) {}
  int size() const { return _n; }
  T *data() { return _data; }
private:
  T *_data;
  int _n;
};

struct Bezier
{
  static Bezier from_points(AsArray<D3DXVECTOR3> points)
  {
    assert(points.size() >= 4);

    // Create a Bezier curve that passes through all the
    // given points.

    // Create a B-spline, and determine the control points
    // that pass throught the given points

    Matrix2d<float> m;
    const int size = points.size() - 2;
    m.init(size, size+1);

    D3DXVECTOR3 *pts = (D3DXVECTOR3 *)_alloca(sizeof(D3DXVECTOR3) * points.size());
    D3DXVECTOR3 *d = points.data();
    pts[0] = d[0];
    pts[points.size()-1] = d[points.size()-1];

    // solve for each coordinate
    for (int c = 0; c < 3; ++c) {

      // build the 1-4-1 matrix
      for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
          m.at(i,j) = 
            (j == i - 1 || j == i + 1) ? 1.0f :
            j == i ? 4.0f :
            0.0f;
        }
      }

      // add the control points to the last column
      for (int i = 0; i < size; ++i)
        m.at(i, size) = 
        i == 0 ? (6*d[1][c] - d[0][c]) :
        i == size - 1 ? (6*d[size][c] - d[size+1][c]) :
        6 * d[i+1][c];

      // solve..
      Matrix2d<float> x;
      gaussian_solve(m, &x);

      // TODO: fix
      for (int i = 0; i < size; ++i)
        pts[i+1][c] = x.at(i,0);

    }

    Bezier b;

    // there are points-1 bezier curves
    for (int i = 0; i < points.size()-1; ++i)
      b.curves.push_back(ControlPoints(
      d[i+0],
      2*pts[i+0]/3 + 1*pts[i+1]/3,
      1*pts[i+0]/3 + 2*pts[i+1]/3,
      d[i+1]));

    return b;
  }

  D3DXVECTOR3 interpolate(float t) const
  {
    // 3*(1-t)*t^2*P2+3*(1-t)^2*t*P1+(1-t)^3*P0+p3*t^3

    int ofs = max(0, min((int)curves.size()-1,(int)t));
    t = max(0, min(1,t - ofs));

    const ControlPoints& pts = curves[ofs];

    const float tt = (1-t);
    const float tt2 = tt*tt;
    const float tt3 = tt2*tt;

    const float t2 = t*t;
    const float t3 = t2*t;

    return tt3 * pts.p0 + 3 * tt2 * t * pts.p1 + 3 * tt * t2 * pts.p2 + t3 * pts.p3;
  }

  D3DXVECTOR3 diff1(float t) const
  {
    // 1st derivate
    // -3*t^2*P2+6*(1-t)*t*P2-6*(1-t)*t*P1+3*(1-t)^2*P1-3*(1-t)^2*P0+3*p3*t^2

    int ofs = max(0, min((int)curves.size()-1,(int)t));
    t = max(0, min(1,t - ofs));

    const ControlPoints& pts = curves[ofs];

    const float tt = (1-t);
    const float tt2 = tt*tt;
    const float tt3 = tt2*tt;

    const float t2 = t*t;
    const float t3 = t2*t;

    return -3*t2*pts.p2 + 6*tt*t*pts.p2 - 6*tt*t*pts.p1 + 3*tt2*pts.p1 - 3*tt2*pts.p0 + 3*pts.p3*t2;
  }

  D3DXVECTOR3 diff2(float t) const
  {
    // 2nd derivate
    // -12*t*P2+6*(1-t)*P2+6*t*P1-12*(1-t)*P1+6*(1-t)*P0+6*p3*t

    int ofs = max(0, min((int)curves.size()-1,(int)t));
    t = max(0, min(1,t - ofs));

    const ControlPoints& pts = curves[ofs];

    const float tt = (1-t);
    const float tt2 = tt*tt;
    const float tt3 = tt2*tt;

    const float t2 = t*t;
    const float t3 = t2*t;

    return 12*t*pts.p2 + t*tt*pts.p2 + 6*t*pts.p1 - 12*tt*pts.p1 + 6*tt*pts.p0 + 6*pts.p3*t;

  }

  struct ControlPoints
  {
    ControlPoints(const D3DXVECTOR3& p0, const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, const D3DXVECTOR3& p3) : p0(p0), p1(p1), p2(p2), p3(p3) {}
    D3DXVECTOR3 p0, p1, p2, p3;
  };

  std::vector<ControlPoints> curves;
};

void TestEffect6::extrude(const Bezier& bezier)
{
  int splits = 20;
  float inc = 1 / (float)(splits - 1);

  const uint32_t spins = 20;

  bool first_point = true;

  D3DXVECTOR3 binormal;
  D3DXVECTOR3 prev_binormal;
  D3DXVECTOR3 prev_tangent;
  D3DXVECTOR3 tangent;
  D3DXVECTOR3 normal;
  D3DXVECTOR3 prev_normal;

  D3DXVECTOR3 *vtx = _verts.map();

  const int32_t num_knots = bezier.curves.size();
  float t = 0;
  for (int32_t i = 0; i < num_knots-1; ++i) {
    for (int32_t j = 0; j < splits; ++j, t += inc) {
      D3DXVECTOR3 cur = bezier.interpolate(t);
      tangent = vec3_normalize(bezier.diff1(t));

      if (first_point) {
        first_point = false;
        normal = vec3_cross(tangent, bezier.diff2(t));
        // if the curvature is 0, choose any normal perpedictular to the tangent
        normal = vec3_normalize(D3DXVec3LengthSq(&normal) < 0.0001f ? find_orthogonal(tangent) : vec3_cross(normal, tangent));
        binormal = vec3_cross(tangent, normal);
      } else {
        // Ken Sloan's method to propagate the reference frame
        normal = vec3_cross(prev_binormal, tangent);
        binormal = vec3_cross(tangent, normal);
      }

      prev_binormal = binormal;
      prev_tangent = tangent;

      const float delta_angle = 2 * (float)D3DX_PI / (float)(spins - 1);
      float cur_angle = 0;
      D3DXVECTOR3 tmp;
      D3DXVECTOR3 tmp2;
      D3DXMATRIX mtx_rot2(kMtxId);

      D3DXVECTOR3 scaled_s = normal;

      for (uint32_t k = 0; k < spins; ++k, cur_angle += delta_angle) {
        D3DXMatrixRotationAxis(&mtx_rot2, &tangent, cur_angle);
        D3DXVec3TransformCoord(&tmp, &scaled_s, &mtx_rot2);
        D3DXVec3TransformNormal(&tmp2, &normal, &mtx_rot2);
        *vtx++ = tmp + cur;
        //vb->add(tmp + cur, tmp2);
      }

/*
      if (last_knot) {
        scaled_s *= ((splits_-1) - j) / (float)splits_;
      }

      for (uint32_t k = 0; k < spins; ++k, cur_angle += delta_angle) {
        D3DXMatrixRotationAxis(&mtx_rot2, &tangent, cur_angle);
        D3DXVec3TransformCoord(&tmp, &scaled_s, &mtx_rot2);
        D3DXVec3TransformNormal(&tmp2, &normal, &mtx_rot2);

        vb->add(tmp + cur, tmp2);
      }
*/
    }
  }
/*
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
*/

  _verts.unmap(vtx);
}

Bezier bezier;

TestEffect6::TestEffect6()
{
  D3DXVECTOR3 pts[] = {
    D3DXVECTOR3(0,0,0),
    D3DXVECTOR3(20,40,0),
    D3DXVECTOR3(-20,80,0),
    D3DXVECTOR3(0,120,100),
  };

  bezier = Bezier::from_points(AsArray<D3DXVECTOR3>(pts, ELEMS_IN_ARRAY(pts)));
}

TestEffect6::~TestEffect6()
{
}

bool TestEffect6::init()
{
  using namespace fastdelegate;

  auto& s = System::instance();
  auto& r = ResourceManager::instance();
  auto& g = Graphics::instance();
  auto* d = Graphics::instance().device();

  RETURN_ON_FAIL_BOOL_E(_verts.create(20*1024));
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/simple.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &TestEffect6::effect_loaded)));
  return true;
}

bool TestEffect6::close()
{
  return true;
}

bool TestEffect6::render()
{
  Graphics& g = Graphics::instance();
  ID3D11Device* device = g.device();
  ID3D11DeviceContext* context = g.context();

  extrude(bezier);

  context->OMSetDepthStencilState(g.default_dss(), g.default_stencil_ref());
  context->OMSetBlendState(g.default_blend_state(), g.default_blend_factors(), g.default_sample_mask());

  D3DXMATRIX mtx;
  const D3DXMATRIX view = App::instance().trackball()->view();
  const D3DXMATRIX proj = App::instance().trackball()->proj();
  D3DXMatrixTranspose(&mtx, &(view * proj));
  _effect->set_vs_variable("mtx", mtx);
  _effect->set_cbuffer();
  _effect->unmap_buffers();

  _effect->set_shaders(context);

  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

  set_vb(context, _verts.get(), _verts.stride);
  context->Draw(_verts.num_verts(), 0);

  return true;
}

bool TestEffect6::load_states(const string2& filename)
{
	return true;
}

void TestEffect6::effect_loaded(EffectWrapper *effect)
{
  _effect.reset(effect);
  InputDesc(). 
    add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
    create(_layout, _effect.get());
}

bool TestEffect6::update(float t, float dt, int num_ticks, float a)
{
  return true;   
}

bool TestEffect6::post_update(float t, float dt, float a)
{
  return true;
}
