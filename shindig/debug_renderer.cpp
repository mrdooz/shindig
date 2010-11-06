#include "stdafx.h"
#include "debug_renderer.hpp"
#include "vector_font.hpp"
#include "font_writer.hpp"
#include "resource_manager.hpp"
#include "system.hpp"
#include "lua_utils.hpp"
#include "camera.hpp"

DebugRenderer *DebugRenderer::_instance = nullptr;

DebugRenderer& DebugRenderer::instance()
{
	if (!_instance)
		_instance = new DebugRenderer();
	return *_instance;
}

DebugRenderer::DebugRenderer()
	: _vector_font(createVectorFont())
	, _enabled(true)
{
}

DebugRenderer::~DebugRenderer()
{
  SAFE_DELETE(_vector_font);
}

void create_unit_sphere(std::vector<PosCol> *sphere_verts)
{
  const int32_t num_lines = 20;
  const float horiz_angle_inc = 2 * (float)D3DX_PI / (num_lines-1);
  const float vert_angle_inc = (float)D3DX_PI / (num_lines-1);
  float vert_angle = 0;
  float radius = 1;
	sphere_verts->clear();
  sphere_verts->reserve(2 * num_lines * num_lines);

  for (int32_t i = 0; i < num_lines; ++i) {
    float horiz_angle = 0;
    const float ofs = radius * cosf(vert_angle);
    const float scaled_radius = radius * sinf(vert_angle);
    D3DXVECTOR3 cur = D3DXVECTOR3(scaled_radius * sinf(horiz_angle), ofs, scaled_radius * cosf(horiz_angle));

    for (int32_t j = 0; j < num_lines; ++j) {
      D3DXVECTOR3 next = D3DXVECTOR3(scaled_radius * sinf(horiz_angle + horiz_angle_inc), ofs, scaled_radius * cosf(horiz_angle + horiz_angle_inc));
      sphere_verts->push_back(PosCol(cur));
      sphere_verts->push_back(PosCol(next));
      cur = next;
      horiz_angle += horiz_angle_inc;
    }
    vert_angle += vert_angle_inc;
  }
}


void DebugRenderer::color_sphere(const D3DXCOLOR& col)
{
  for (int32_t i = 0, e = _sphere_verts.size(); i < e; ++i) {
    _sphere_verts[i].col = col;
  }
}

void  DebugRenderer::add_line(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXCOLOR& color, const D3DXMATRIX& view_proj)
{
  PosCol pts[2];
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
  add_verts(Pos | Color, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, _sphere_verts[0].pos, _sphere_verts.size(), world2 * world, view_proj);
}

void DebugRenderer::add_wireframe_sphere(const D3DXMATRIX& world, const D3DXCOLOR& color, const D3DXMATRIX& view_proj)
{
  color_sphere(color);
  add_verts(Pos | Color, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, _sphere_verts[0].pos, _sphere_verts.size(), world, view_proj);
}

bool DebugRenderer::init()
{
	using namespace fastdelegate;

	auto& v = Graphics::instance().viewport();
	auto& s = System::instance();
	auto& r = ResourceManager::instance();

	_font_writer.reset(new FontWriter());
	RETURN_ON_FAIL_BOOL_E(_font_writer->init(s.convert_path("data/fonts/arial.ttf", System::kDirRelative), 0, 0, 600, 600));
	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/debug_renderer_states.lua", System::kDirRelative), MakeDelegate(this, &DebugRenderer::load_states), true));
  RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_renderer.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &DebugRenderer::load_effect)));
  RETURN_ON_FAIL_BOOL_E(_verts.create(1000000));

  create_unit_sphere(&_sphere_verts);

  if (!init_vertex_buffers())
    return false;

  return true;
}

bool DebugRenderer::init_vertex_buffers()
{
  const uint32_t num_verts = 200 * 1000;

  // pos
/*
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
*/
  // pos color
  {
    VertexFormatData& data = vertex_formats_[Pos + Color];
    data.vertex_size_ = sizeof(D3DXVECTOR3) + sizeof(D3DXCOLOR);
    data.num_verts = num_verts;
    data.buffer_size_ = data.vertex_size_ * data.num_verts;

    ID3D11Device* device = Graphics::instance().device();
    create_dynamic_vertex_buffer(device, num_verts, data.vertex_size_, &data._vertex_buffer);

    //data.technique_name_ = "render_pos_color";
/*
    if (!effect_->get_pass_desc(pass_desc, data.technique_name_)) {
      return false;
    }

    if (!create_input_layout<mpl::vector<D3DXVECTOR3, D3DXCOLOR> >(data.input_layout_, pass_desc, _device)) {
      return false;
    }
*/
  }

  return true;
}

bool DebugRenderer::close()
{
  delete this;
  _instance = nullptr;
  return true;
}

void DebugRenderer::start_frame()
{
  ID3D11DeviceContext* context = Graphics::instance().context();

  for (auto i = vertex_formats_.begin(), e = vertex_formats_.end(); i != e; ++i) {
    VertexFormatData& cur = i->second;
    D3D11_MAPPED_SUBRESOURCE r;
    if (FAILED(context->Map(cur._vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &r))) {
      LOG_WARNING_LN("Error mapping vertex buffer");
      return;
    }
    cur.draw_calls_by_topology_.clear();
    cur._data_ofs = 0;
    cur.vertex_count_ = 0;
    cur._data = (uint8_t *)r.pData;
  }

  debug_text_.clear();
}

void DebugRenderer::end_frame()
{
  ID3D11DeviceContext* context = Graphics::instance().context();
  for (auto i = vertex_formats_.begin(), e = vertex_formats_.end(); i != e; ++i) {
    VertexFormatData& cur = i->second;
    context->Unmap(cur._vertex_buffer, 0);
  }

}

void DebugRenderer::add_verts(const uint32_t vertex_format, const D3D11_PRIMITIVE_TOPOLOGY topology, 
                              const float* verts, const uint32_t vertex_count, const D3DXMATRIX& world, const D3DXMATRIX& view_proj)
{
  VertexFormatData& cur = vertex_formats_[vertex_format];

  const uint32_t data_size = cur.vertex_size_ * vertex_count;
  if (cur._data_ofs + data_size > cur.buffer_size_) {
    LOG_WARNING_LN("Too much data passed to add_verts");
    return;
  }
#if _DEBUG
  assert(!IsBadWritePtr(&cur._data[cur._data_ofs], data_size));
  assert(!IsBadReadPtr(verts, data_size));
#endif
  memcpy(&cur._data[cur._data_ofs], (void*)verts, data_size);
  D3DXVec3TransformCoordArray(
    (D3DXVECTOR3*)&cur._data[cur._data_ofs], cur.vertex_size_, (D3DXVECTOR3*)verts, cur.vertex_size_, &world, vertex_count);

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
    cur.draw_calls_by_topology_[topology].push_back(DrawCall(vertex_count, cur._data_ofs / cur.vertex_size_, view_proj));
  }
  cur._data_ofs += data_size;
  cur.vertex_count_ += vertex_count;
}

void DebugRenderer::render()
{
	if (!_enabled)
		return;

  ID3D11Device* device = Graphics::instance().device();
  ID3D11DeviceContext* context = Graphics::instance().context();

  PosCol *vtx = _verts.map();

  DebugDraw d;
	int idx = 0;
	for (auto i = _debug_render_delegates.begin(), e = _debug_render_delegates.end(); i != e; ++i) {
		(*i)(&d);
		++idx;

    for (int j = 0; j < (int)_sphere_verts.size(); ++j) {
      *vtx++ = PosCol(d.sphere.radius * _sphere_verts[j].pos + d.sphere.center, D3DXCOLOR(1,1,1,1));
    }
	}

  int vertex_count = _verts.unmap(vtx);

  D3DXMATRIX mtx, view, proj;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0,0,-100), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
	D3DXMatrixPerspectiveFovLH(&proj, deg_to_rad(45), 4/3.0f, 1, 1000);
  D3DXMatrixTranspose(&mtx, &(view * proj));
  _effect->set_vs_variable("mtx", mtx);
  _effect->set_cbuffer();
  _effect->set_shaders(context);

  float blend_factors[] = {1, 1, 1, 1};
  context->OMSetDepthStencilState(_dss, 0);

  float blend_factor[] = { 1, 1, 1, 1 };
  context->OMSetBlendState(_blend_state, blend_factor, 0xffffffff);

  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  set_vb(context, _verts.get(), _verts.stride);
  context->Draw(vertex_count, 0);


  //float blend_factor[] = {0, 0, 0, 0};
  //device->OMSetBlendState(blend_state_, blend_factor, 0xffffffff);
  //device->OMSetDepthStencilState(depth_stencil_state_, 0xffffffff);

  D3DXMATRIX prev_mtx;
  ZeroMemory(&prev_mtx, sizeof(prev_mtx));

  // Draw the 3d debug stuff
  for (VertexFormats::iterator i = vertex_formats_.begin(), e = vertex_formats_.end(); i != e; ++i) {
    VertexFormatData& cur = i->second;

    if (cur.vertex_count_ > 0) {
      const UINT offset = 0;
      //effect_->set_technique(cur.technique_name_);
      //_device->IASetInputLayout(cur.input_layout_);
      ID3D11Buffer* bufs[] = { cur._vertex_buffer };
      uint32_t strides[] = { cur.vertex_size_ };
      set_vb(context, cur._vertex_buffer, cur.vertex_size_);
      //_device->IASetVertexBuffers(0, 1, bufs, strides, &offset);

      typedef DrawCallsByTopology::iterator It;
      for (It i_top = cur.draw_calls_by_topology_.begin(), e_top = cur.draw_calls_by_topology_.end(); i_top != e_top; ++i_top) {

        //_device->IASetPrimitiveTopology(i_top->first);

        typedef DrawCalls::iterator CallsIt;
        for (CallsIt i_call = i_top->second.begin(), e_call = i_top->second.end(); i_call != e_call; ++i_call) {
          const D3DXMATRIX& view_proj = i_call->view_proj;
          if (prev_mtx != view_proj) {
            D3DXMATRIX mtx;
            D3DXMatrixTranspose(&mtx, &view_proj);
            _effect->set_vs_variable("mtx", mtx);
            _effect->set_cbuffer();
            _effect->unmap_buffers();

            //effect_->set_variable("view_proj", view_proj);
            //effect_->set_technique(cur.technique_name_);
            prev_mtx = view_proj;
          }
          context->Draw(i_call->vertex_count, i_call->start_vertex_location);
        }
      }
    }
  }
/*
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
  _vector_font->vprintf(verts, 1, false, buf);
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

void DebugRenderer::add_debug_render_delegate(const DebugRenderDelegate& d, bool add)
{
	if (add) {
		_debug_render_delegates.push_back(d);
		return;
	} else {
		for (auto i = _debug_render_delegates.begin(), e = _debug_render_delegates.end(); i != e; ++i) {
			if ((*i) == d) {
				_debug_render_delegates.erase(i);
				return;
			}
		}
	}
}

void DebugRenderer::add_debug_camera_delegate(const DebugCameraDelegate& d, bool add)
{
	if (add) {
		_debug_camera_delegates.push_back(d);
	} else {
		for (auto i = _debug_camera_delegates.begin(), e = _debug_camera_delegates.end(); i != e; ++i) {
			if (*i == d) {
				_debug_camera_delegates.erase(i);
				return;
			}
		}

	}
}

void DebugRenderer::begin_debug_draw()
{

}

void DebugRenderer::end_debug_draw()
{

}

bool DebugRenderer::load_states(const string2& filename)
{
	auto& s = System::instance();
	if (!lua_load_states(filename, "default_blend", "default_dss", NULL, &_blend_state.p, &_dss.p, NULL))
		return false;

	return true;
}

void DebugRenderer::load_effect(EffectWrapper *effect)
{
	_effect.reset(effect);

	InputDesc(). 
		add("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
		create(_layout, _effect.get());
}

void DebugRenderer::draw_plane(const Camera *cam)
{
  // find 4 corners of the plane clipped to the camera frustum
  // 0--1
  // 2--3

  D3DXVECTOR3 v0, v1, v2, v3;

  const float a = tanf(cam->fov());
  v0.x = -cam->far_plane() * a * cam->aspect_ratio();
  v1.x = +cam->far_plane() * a * cam->aspect_ratio();
  v2.x = -cam->near_plane() * a * cam->aspect_ratio();
  v3.x = +cam->near_plane() * a * cam->aspect_ratio();

  v0.z = cam->far_plane();
  v1.z = cam->far_plane();
  v2.z = cam->near_plane();
  v3.z = cam->near_plane();

  const float y = -50;
  v0.y = v1.y = v2.y = v3.y = -50;

  // transform to world space
  D3DXMATRIX to_world;
  D3DXMatrixInverse(&to_world, NULL, &cam->view());

  D3DXMATRIX view_proj = cam->view() * cam->proj();

  const int steps = 10;
  const float z_inc = (v0.z - v2.z) / steps;
  float z_cur = v3.z;
  for (int i = 0; i <= steps; ++i) {
    const float x_inc = (v1.x - v3.x) / steps;
    float x_cur = v3.x;
    for (int j = 0; j <= steps; ++j) {
      add_line(D3DXVECTOR3(-x_cur, v0.y, z_cur), D3DXVECTOR3(+x_cur, v0.y, z_cur), D3DXCOLOR(1,1,1,1), view_proj);
      x_cur += x_inc;
    }
    z_cur += z_inc;
  }
}
