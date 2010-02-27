#include "stdafx.h"
#include "debug_renderer.hpp"
#include "vector_font.hpp"
//#include <celsus/D3D11Descriptions.hpp>

//namespace mpl = boost::mpl;


const D3DXMATRIX kMtxId(1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0,1);

const D3DXVECTOR3 kVec3Zero(0,0,0);
const D3DXVECTOR3 kVec3One(1,1,1);
const D3DXQUATERNION kQuatId(0, 0, 0, 1);

inline D3DXMATRIX get_rotation(const D3DXMATRIX& mtx)
{
  return D3DXMATRIX(
    mtx._11, mtx._12, mtx._13, 0,
    mtx._21, mtx._22, mtx._23, 0,
    mtx._31, mtx._32, mtx._33, 0,
    0, 0, 0, 1);
}

inline D3DXVECTOR3 get_translation(const D3DXMATRIX& transform)
{
  return D3DXVECTOR3(transform._41, transform._42, transform._43);
}

inline D3DXVECTOR3 get_scale(const D3DXMATRIX& transform)
{
  return D3DXVECTOR3(transform._11, transform._22, transform._33);
}

inline D3DXVECTOR3 normalize(const D3DXVECTOR3& a)
{
  const float len = D3DXVec3Length(&a);
  if (len > 0 ) {
    return a / len;
  }
  return kVec3Zero;
}

namespace
{
  struct VtxCol
  {
    VtxCol() {}
    VtxCol(const D3DXVECTOR3& pos, const D3DXCOLOR& col) : pos(pos), col(col) {}
    D3DXVECTOR3 pos;
    D3DXCOLOR col;
  };

}

DebugRenderer::DebugRenderer(const ID3D11DevicePtr& device)
  : _device(device)
  //, effect_(new EffectWrapper(device))
  , vector_font_(createVectorFont())
{
/*
  D3DX10CreateFont( _device, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
    OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
    _T("Arial"), &font_);
*/
}

DebugRenderer::~DebugRenderer()
{
  SAFE_DELETE(vector_font_);
//  SAFE_DELETE(effect_);
}

void DebugRenderer::init_unit_sphere()
{
  const int32_t num_lines = 20;
  const float horiz_angle_inc = 2 * (float)D3DX_PI / (num_lines-1);
  const float vert_angle_inc = (float)D3DX_PI / (num_lines-1);
  float vert_angle = 0;
  float radius = 1;
  sphere_verts_.reserve(2 * num_lines * num_lines);

  for (int32_t i = 0; i < num_lines; ++i) {
    float horiz_angle = 0;
    const float ofs = radius * cosf(vert_angle);
    const float scaled_radius = radius * sinf(vert_angle);
    D3DXVECTOR3 cur = D3DXVECTOR3(scaled_radius * sinf(horiz_angle), ofs, scaled_radius * cosf(horiz_angle));

    for (int32_t j = 0; j < num_lines; ++j) {
      D3DXVECTOR3 next = D3DXVECTOR3(scaled_radius * sinf(horiz_angle + horiz_angle_inc), ofs, scaled_radius * cosf(horiz_angle + horiz_angle_inc));
      sphere_verts_.push_back(PosCol(cur));
      sphere_verts_.push_back(PosCol(next));
      cur = next;
      horiz_angle += horiz_angle_inc;
    }
    vert_angle += vert_angle_inc;
  }
}


void DebugRenderer::color_sphere(const D3DXCOLOR& col)
{
  for (int32_t i = 0, e = sphere_verts_.size(); i < e; ++i) {
    sphere_verts_[i].col = col;
  }
}

void  DebugRenderer::add_line(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXCOLOR& color, const D3DXMATRIX& view_proj)
{
  VtxCol pts[2];
  pts[0].pos = a;
  pts[0].col = color;

  pts[1].pos = b;
  pts[1].col = color;

  add_verts(Pos | Color, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, &pts[0].pos.x, 2, kMtxId, view_proj);
}

void DebugRenderer::add_wireframe_sphere(const D3DXVECTOR3& center, const float radius, const D3DXCOLOR& color, 
                                         const D3DXMATRIX& world, const D3DXMATRIX& view_proj)
{
  D3DXMATRIX world2;
  D3DXMatrixAffineTransformation(&world2, radius, &kVec3Zero, &kQuatId, &center);
  color_sphere(color);
  add_verts(Pos | Color, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, sphere_verts_[0].pos, sphere_verts_.size(), world2 * world, view_proj);
}

void DebugRenderer::add_wireframe_sphere(const D3DXMATRIX& world, const D3DXCOLOR& color, const D3DXMATRIX& view_proj)
{
  color_sphere(color);
  add_verts(Pos | Color, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, sphere_verts_[0].pos, sphere_verts_.size(), world, view_proj);
}

ID3D11Buffer* DebugRenderer::create_dynamic_vertex_buffer(const uint32_t vertex_count, const uint32_t vertex_size)
{
  const uint32_t buffer_size = vertex_count * vertex_size;

  D3D11_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));
  buffer_desc.Usage     = D3D11_USAGE_DYNAMIC;
  buffer_desc.ByteWidth = buffer_size;
  buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  ID3D11Buffer* vertex_buffer = NULL;
  if (FAILED(_device->CreateBuffer(&buffer_desc, NULL, &vertex_buffer))) {
    return NULL;
  }
  return vertex_buffer;
}

bool DebugRenderer::init()
{
/*
  if (!effect_->load("effects/DebugRenderer.fx")) {
    return false;
  }

  blend_state_.Attach(rt::D3D11::BlendDescription()
    .BlendEnable_(0, FALSE)
    .Create(_device));

  depth_stencil_state_.Attach(rt::D3D11::DepthStencilDescription()
    .Create(_device));

  if (!init_vertex_buffers()) {
    return false;
  }
*/
  init_unit_sphere();

  return true;
}

bool DebugRenderer::init_vertex_buffers()
{
  // pos
/*
  const uint32_t num_verts = 200 * 1000;
  D3D11_PASS_DESC pass_desc;
  {
    VertexFormatData& data = vertex_formats_[Pos];
    data.vertex_size_ = sizeof(D3DXVECTOR3);
    data.num_verts = num_verts;
    data.buffer_size_ = data.vertex_size_ * data.num_verts;
    data._vertex_buffer = create_dynamic_vertex_buffer(data.num_verts, data.vertex_size_);
    data.technique_name_ = "render_pos";

    if (!effect_->get_pass_desc(pass_desc, data.technique_name_)) {
      return false;
    }

    if (!create_input_layout<mpl::vector<D3DXVECTOR3> >(data.input_layout_, pass_desc, _device)) {
      return false;
    }
  }

  // pos color
  {
    VertexFormatData& data = vertex_formats_[Pos + Color];
    data.vertex_size_ = sizeof(D3DXVECTOR3) + sizeof(D3DXCOLOR);
    data.num_verts = num_verts;
    data.buffer_size_ = data.vertex_size_ * data.num_verts;
    data._vertex_buffer = create_dynamic_vertex_buffer(data.num_verts, data.vertex_size_);
    data.technique_name_ = "render_pos_color";

    if (!effect_->get_pass_desc(pass_desc, data.technique_name_)) {
      return false;
    }

    if (!create_input_layout<mpl::vector<D3DXVECTOR3, D3DXCOLOR> >(data.input_layout_, pass_desc, _device)) {
      return false;
    }
  }
*/
  return true;
}

void DebugRenderer::close()
{
}

void DebugRenderer::start_frame()
{
/*
  for (VertexFormats::iterator i = vertex_formats_.begin(), e = vertex_formats_.end(); i != e; ++i) {
    VertexFormatData& cur = i->second;
    if (FAILED(cur._vertex_buffer->Map(D3D11_MAP_WRITE_DISCARD, 0, (void**)&cur.data_))) {
      LOG_WARNING_LN("Error mapping vertex buffer");
    }
    cur.draw_calls_by_topology_.clear();
    cur.data_ofs_ = 0;
    cur.vertex_count_ = 0;
  }
*/
  debug_text_.clear();
}

void DebugRenderer::end_frame()
{
/*
  for (VertexFormats::iterator i = vertex_formats_.begin(), e = vertex_formats_.end(); i != e; ++i) {
    VertexFormatData& cur = i->second;
    cur._vertex_buffer->Unmap();
  }
*/
}

void DebugRenderer::add_verts(const uint32_t vertex_format, const D3D11_PRIMITIVE_TOPOLOGY topology, 
                              const float* verts, const uint32_t vertex_count, const D3DXMATRIX& world, const D3DXMATRIX& view_proj)
{
  VertexFormatData& cur = vertex_formats_[vertex_format];

  const uint32_t data_size = cur.vertex_size_ * vertex_count;
  if (cur.data_ofs_ + data_size > cur.buffer_size_) {
    LOG_WARNING_LN("Too much data passed to add_verts");
    return;
  }
#if _DEBUG
  assert(!IsBadWritePtr(&cur.data_[cur.data_ofs_], data_size));
  assert(!IsBadReadPtr(verts, data_size));
#endif
  memcpy(&cur.data_[cur.data_ofs_], (void*)verts, data_size);
  D3DXVec3TransformCoordArray(
    (D3DXVECTOR3*)&cur.data_[cur.data_ofs_], cur.vertex_size_, (D3DXVECTOR3*)verts, cur.vertex_size_, &world, vertex_count);

  // if this is a list format, we might be able to stitch it together with the previous call
  const bool list_format = topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST || topology == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  bool stitched = false;

  if (list_format) {
    if (cur.draw_calls_by_topology_.find(topology) != cur.draw_calls_by_topology_.end()) {
      DrawCalls& draw_calls = cur.draw_calls_by_topology_[topology];
      DrawCall& last_call = draw_calls.back();
      if (last_call.view_proj == view_proj) {
        last_call.vertex_count += vertex_count;
        stitched = true;
      }
    }
  }

  if (!stitched) {
    cur.draw_calls_by_topology_[topology].push_back(DrawCall(vertex_count, cur.data_ofs_ / cur.vertex_size_, view_proj));
  }
  cur.data_ofs_ += data_size;
  cur.vertex_count_ += vertex_count;
}

void DebugRenderer::render()
{
/*
  float blend_factor[] = {0, 0, 0, 0};
  _device->OMSetBlendState(blend_state_, blend_factor, 0xffffffff);
  _device->OMSetDepthStencilState(depth_stencil_state_, 0xffffffff);

  D3DXMATRIX prev_mtx;
  ZeroMemory(&prev_mtx, sizeof(prev_mtx));

  // Draw the 3d debug stuff
  for (VertexFormats::iterator i = vertex_formats_.begin(), e = vertex_formats_.end(); i != e; ++i) {
    VertexFormatData& cur = i->second;

    if (cur.vertex_count_ > 0) {
      const UINT offset = 0;
      effect_->set_technique(cur.technique_name_);
      _device->IASetInputLayout(cur.input_layout_);
      ID3D11Buffer* bufs[] = { cur._vertex_buffer };
      uint32_t strides[] = { cur.vertex_size_ };
      _device->IASetVertexBuffers(0, 1, bufs, strides, &offset);

      typedef DrawCallsByTopology::iterator It;
      for (It i_top = cur.draw_calls_by_topology_.begin(), e_top = cur.draw_calls_by_topology_.end(); i_top != e_top; ++i_top) {

        _device->IASetPrimitiveTopology(i_top->first);

        typedef DrawCalls::iterator CallsIt;
        for (CallsIt i_call = i_top->second.begin(), e_call = i_top->second.end(); i_call != e_call; ++i_call) {
          const D3DXMATRIX& view_proj = i_call->view_proj;
          if (prev_mtx != view_proj) {
            effect_->set_variable("view_proj", view_proj);
            effect_->set_technique(cur.technique_name_);
            prev_mtx = view_proj;
          }
          _device->Draw(i_call->vertex_count, i_call->start_vertex_location);
        }
      }
    }
  }

  // Write the debug text
  std::string all_text;
  for (size_t i = 0, e = debug_text_.size(); i < e; ++i) {
    all_text += debug_text_[i] + "\n";
  }

  RECT rect;
  rect.top = 0;
  rect.bottom = 1000;
  rect.left = 400;
  rect.right = 800;
  font_->DrawText(NULL, all_text.c_str(), -1, &rect, DT_WORDBREAK , D3DXCOLOR(1, 1, 1, 1));
*/
}

void DebugRenderer::add_text(const D3DXVECTOR3& pos, const D3DXMATRIX& view, const D3DXMATRIX& proj, const bool billboard, const char* format, ...)
{
  D3DXMATRIX trans;
  D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);

  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  std::vector<float> verts;
  vector_font_->vprintf(verts, 1, false, buf);
  if (billboard) {
    D3DXMATRIX rot = get_rotation(view);
    D3DXMatrixTranspose(&rot, &rot);
    add_verts(Pos, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, &verts[0], verts.size() / 3, kMtxId, rot * trans * view * proj);
  } else {
    add_verts(Pos, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, &verts[0], verts.size() / 3, kMtxId, trans * view * proj);
  }
}

void DebugRenderer::add_debug_string(const char* format, ...)
{
  va_list arg;
  va_start(arg, format);

  const int len = _vscprintf(format, arg) + 1;

  char* buf = (char*)_alloca(len);
  vsprintf_s(buf, len, format, arg);
  va_end(arg);

  debug_text_.push_back(buf);
}
