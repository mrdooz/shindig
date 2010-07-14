#include "stdafx.h"
#include "debug_renderer.hpp"
#include "vector_font.hpp"
#include "font_writer.hpp"
#include "resource_manager.hpp"
#include "system.hpp"
#include "lua_utils.hpp"

DebugRenderer *DebugRenderer::_instance = nullptr;

DebugRenderer& DebugRenderer::instance()
{
	if (!_instance)
		_instance = new DebugRenderer();
	return *_instance;
}

DebugRenderer::DebugRenderer()
	: _vector_font(createVectorFont())
{
/*
  D3DX10CreateFont( _device, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
    OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
    _T("Arial"), &font_);
*/
}

DebugRenderer::~DebugRenderer()
{
  SAFE_DELETE(_vector_font);
	SAFE_DELETE(_font_writer);
//  SAFE_DELETE(effect_);
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

	_font_writer = new FontWriter();
	RETURN_ON_FAIL_BOOL_E(_font_writer->init(s.convert_path("data/fonts/arial.ttf", System::kDirRelative), 0, 0, 600, 600));
	RETURN_ON_FAIL_BOOL_E(s.add_file_changed(s.convert_path("data/scripts/debug_renderer_states.lua", System::kDirRelative), MakeDelegate(this, &DebugRenderer::load_states), true));
	RETURN_ON_FAIL_BOOL_E(r.load_shaders(s.convert_path("effects/debug_renderer.fx", System::kDirRelative), "vsMain", NULL, "psMain", MakeDelegate(this, &DebugRenderer::load_effect)));



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
	create_unit_sphere(&_sphere_verts);

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

bool DebugRenderer::close()
{
  return true;
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
	for (auto i = _debug_render_delegates.begin(), e = _debug_render_delegates.end(); i != e; ++i) {
		DebugDraw d;
		(*i)(&d);
		int a = 10;
	}
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
		add("SV_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0).
		add("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0).
		create(_layout, _effect.get());
}
