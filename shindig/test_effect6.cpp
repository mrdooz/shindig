#include "stdafx.h"
#include "test_effect6.hpp"
#include "system.hpp"
#include "app.hpp"
#include "debug_menu.hpp"
#include "lua_utils.hpp"
#include "debug_renderer.hpp"
#include "camera.hpp"

using namespace std;


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

Bezier Bezier::from_points(AsArray<D3DXVECTOR3> data_points)
{
  assert(data_points.size() >= 4);
  D3DXVECTOR3 *s = data_points.data();

  // S are data points. We want to calculate the control
  // points (B) that give us a Bezier curve that passes through S

  // Create a B-spline, and determine the control points
  // that pass throught the given points

  Matrix2d<float> m;
  const int size = data_points.size() - 2;
  m.init(size, size+1);

  vector<D3DXVECTOR3> b;
  b.resize(data_points.size());
  const int back = data_points.size()-1;
  b[0] = s[0];
  b[back] = s[back];

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
    m.at(0,size) = (6*s[1][c] - s[0][c]);
    m.at(size-1,size) = (6*s[size][c] - s[size+1][c]);
    for (int i = 1; i < size-1; ++i)
      m.at(i,size) = 6 * s[i+1][c];

    // solve..
    Matrix2d<float> x;
    gaussian_solve(m, &x);

    // TODO: fix
    for (int i = 0; i < size; ++i)
      b[i+1][c] = x.at(i,0);

  }

  Bezier bezier;

  // There are [data_points-1] bezier segments. Create these from
  // the B-spline control points
  for (int i = 0; i < data_points.size()-1; ++i)
    bezier.curves.push_back(ControlPoints(
    s[i+0],
    2*b[i+0]/3 + 1*b[i+1]/3,
    1*b[i+0]/3 + 2*b[i+1]/3,
    s[i+1]));

  return bezier;
}

D3DXVECTOR3 Bezier::interpolate(float t) const
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

D3DXVECTOR3 Bezier::diff1(float t) const
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

D3DXVECTOR3 Bezier::diff2(float t) const
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

void TestEffect6::extrude(const Bezier& bezier)
{
  const int splits = 20;
  float inc = 1 / (float)(splits - 1);

  const uint32_t spins = 20;

  static bool first_point = true;

  static D3DXVECTOR3 prev_binormal;
  const int32_t num_segments = bezier.curves.size();

  vector<PosNormal> verts;
  verts.reserve(num_segments*splits*spins);

  const float max_t = (float)num_segments;
  const float inc_t = 1.0f / splits;
  float t = 0;
  while (t <= max_t) {
    D3DXVECTOR3 cur = bezier.interpolate(t);
    static D3DXVECTOR3 tangent = vec3_normalize(bezier.diff1(t));

    D3DXVECTOR3 binormal;
    D3DXVECTOR3 normal;
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
      verts.push_back(PosNormal(tmp + cur, tmp2));
    }
    t += inc_t;
  }

  PosNormal *vtx = _verts.map();
  const int segments = verts.size() / spins - 1;
  for (int i = 0; i < segments; ++i) {
    for (int j = 0; j < spins; ++j) {
      const PosNormal& v0 = verts[(i+0)*spins+j];
      const PosNormal& v1 = verts[(i+1)*spins+j];
      const PosNormal& v2 = verts[(i+1)*spins+((j+1)%spins)];
      const PosNormal& v3 = verts[(i+0)*spins+((j+1)%spins)];
      *vtx++ = v0;
      *vtx++ = v2;
      *vtx++ = v1;

      *vtx++ = v0;
      *vtx++ = v3;
      *vtx++ = v2;
    }
  }
  _verts.unmap(vtx);
}

void TestEffect6::extrude2(const Bezier& bezier)
{
  // uses the progress to determine how much to extrude

  const int num_segments = bezier.curves.size();
  const int splits = 20;

  const uint32_t spins = 20;

  static bool first_point = true;

  static D3DXVECTOR3 prev_binormal;

  vector<PosNormal> verts;
  verts.reserve(num_segments*splits*spins);

  const float max_t = _progress * num_segments;
  const float inc_t = 1.0f / splits;
  float t = 0;
  while (t <= max_t) {
    D3DXVECTOR3 cur = bezier.interpolate(t);
    static D3DXVECTOR3 tangent = vec3_normalize(bezier.diff1(t));

    D3DXVECTOR3 binormal;
    D3DXVECTOR3 normal;
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
      verts.push_back(PosNormal(tmp + cur, tmp2));
    }
    t += inc_t;
  }

  PosNormal *vtx = _tmp_verts.map();
  const int segments = verts.size() / spins - 1;
  for (int i = 0; i < segments; ++i) {
    for (int j = 0; j < spins; ++j) {
      const PosNormal& v0 = verts[(i+0)*spins+j];
      const PosNormal& v1 = verts[(i+1)*spins+j];
      const PosNormal& v2 = verts[(i+1)*spins+((j+1)%spins)];
      const PosNormal& v3 = verts[(i+0)*spins+((j+1)%spins)];
      *vtx++ = v0;
      *vtx++ = v2;
      *vtx++ = v1;

      *vtx++ = v0;
      *vtx++ = v3;
      *vtx++ = v2;
    }
  }
  _tmp_verts.unmap(vtx);
}

Bezier bezier;

TestEffect6::TestEffect6()
  : _up(0,1,0)
  , _last_update(0)
  , _cur_top(0,0,0)
  , _angle(0)
{
}

TestEffect6::~TestEffect6()
{
}

struct PlyLoader
{
  bool load(const char *filename);
  void parse_header();

  AsArray<byte> _data;
};

bool PlyLoader::load(const char *filename)
{
  if (!load_file(filename, &_data))
    return false;

  return true;
}

void PlyLoader::parse_header()
{

}


bool TestEffect6::init()
{
  auto& s = System::instance();
  auto& r = ResourceManager::instance();

  RETURN_ON_FAIL_BOOL_E(_verts.create(1000000));
  RETURN_ON_FAIL_BOOL_E(_tmp_verts.create(20000));
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/test_effect6.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &TestEffect6::effect_loaded)));
  App::instance().add_update_callback(MakeDelegate(this, &TestEffect6::update), true);


  D3DXMATRIX mtx;
  const float len = 1;
  vector<D3DXVECTOR3> pts;
  D3DXVECTOR3 up(0,1,0), cur_top(0,0,0);
  float angle = 0;
  for (int j = 0; j < 100; ++j) {
    for (int i = 0; i < 4; ++i) {
      // each point is selected as a random point on the circle around
      // the current normal

      D3DXVECTOR3 pt = cur_top + len * _up + D3DXVECTOR3(0.1f, 0, 0);
      //angle += randf(-(float)D3DX_PI/100, (float)D3DX_PI/100);
      D3DXMatrixRotationAxis(&mtx, &up, angle);
      D3DXVec3TransformCoord(&pt, &pt, &mtx);
      pts.push_back(pt);
      up = vec3_normalize(pt - cur_top);
      cur_top = pt;
    }

  }

  _full_curve = Bezier::from_points(AsArray<D3DXVECTOR3>(pts));

  return true;
}

bool TestEffect6::close()
{
  App::instance().add_update_callback(MakeDelegate(this, &TestEffect6::update), false);

  return true;
}

bool TestEffect6::render()
{
  if (!bezier.curves.empty())
    extrude2(bezier);

  Graphics& g = Graphics::instance();
  ID3D11Device* device = g.device();
  ID3D11DeviceContext* context = g.context();

  context->OMSetDepthStencilState(g.default_depth_stencil_state(), g.default_stencil_ref());
  context->OMSetBlendState(g.default_blend_state(), g.default_blend_factors(), g.default_sample_mask());
  context->RSSetState(g.default_rasterizer_state());

  D3DXMATRIX mtx;
  const D3DXMATRIX view = App::instance().camera()->view();
  const D3DXMATRIX proj = App::instance().camera()->proj();
  D3DXMatrixTranspose(&mtx, &(view * proj));
  _effect->set_vs_variable("mtx", mtx);
  _effect->set_cbuffer();
  _effect->unmap_buffers();

  _effect->set_shaders(context);

  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  set_vb(context, _verts.get(), _verts.stride);
  context->Draw(_verts.num_verts(), 0);

  set_vb(context, _tmp_verts.get(), _tmp_verts.stride);
  context->Draw(_tmp_verts.num_verts(), 0);

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
    add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0).
    add("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12).
    create(_layout, _effect.get());
}

void TestEffect6::update(float t, float dt, int num_ticks, float a)
{

  _progress = min(1, max(0, (t - _last_update)));

  // every second, add a new curve
  if (t - _last_update > 1.0f) {

    // add the previous bezier segments
    if (!bezier.curves.empty())
      extrude(bezier);

    _last_update = t;
    _progress = 0;

    bezier.curves.clear();
    int ofs = max(0,(int)t-1);
    int stride = 2;
    for (int i = 0; i < stride; ++i)
      bezier.curves.push_back(_full_curve.curves[ofs*stride+i]);
/*
    D3DXMATRIX mtx;
    const float len = 2;
    _cur_points.clear();
    _cur_points.push_back(_cur_top);
    for (int i = 0; i < 4; ++i) {
      // each point is selected as a random point on the circle around
      // the current normal

      D3DXVECTOR3 pt = _cur_top + len * _up + D3DXVECTOR3(0.1f, 0, 0);
      _angle += randf(-(float)D3DX_PI/100, (float)D3DX_PI/100);
      D3DXMatrixRotationAxis(&mtx, &_up, _angle);
      D3DXVec3TransformCoord(&pt, &pt, &mtx);
      _cur_points.push_back(pt);
      _up = vec3_normalize(pt - _cur_top);
      _cur_top = pt;
    }
*/
    //bezier = Bezier::from_points(AsArray<D3DXVECTOR3>(_cur_points));
  }
}
