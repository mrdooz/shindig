#ifndef DEBUG_RENDERER_HPP
#define DEBUG_RENDERER_HPP

#include "vector_font.hpp"
#include <d3dx10math.h>
#include <celsus/vertex_types.hpp>

class EffectWrapper;

typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;
typedef CComPtr<ID3D11InputLayout> ID3D11InputLayoutPtr;
typedef CComPtr<ID3D11Device> ID3D11DevicePtr;


// objects that want to be debug drawn can implement this
struct DebugDraw
{
  enum Bounding {
    kSphere = 1 << 0,
    kBox = 1 << 1,
  };

  string2 name;
  D3DXMATRIX orientation;

  D3DXVECTOR3 center;
  float radius;
};

struct DebugCamera
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 lookat;
	D3DXVECTOR3 up;
	float fov;
	float near_plane, far_plane;
};

typedef fastdelegate::FastDelegate1<DebugDraw> DebugRenderDelegate;
typedef fastdelegate::FastDelegate1<DebugCamera> DebugCameraDelegate;


class DebugRenderer
{
public:
  enum {
    Pos = 1 << 0,
    Normal = 1 << 1,
    Color = 1 << 2,
  };


	static DebugRenderer& instance();

	void add_debug_render_delegate(const DebugRenderDelegate& d, bool add);
	void add_debug_camera_delegate(const DebugCameraDelegate& d, bool add);

  bool init();
  void close();
  void start_frame();
  void end_frame();
  // vertex_format is a bitmask made from Pos, Normal, Color
  void add_verts(const uint32_t vertex_format, const D3D11_PRIMITIVE_TOPOLOGY topology, const float* verts, const uint32_t vertex_count, 
    const D3DXMATRIX& world, const D3DXMATRIX& view_proj);
  void render();

  void add_wireframe_sphere(const D3DXVECTOR3& center, const float radius, const D3DXCOLOR& color, const D3DXMATRIX& world, const D3DXMATRIX& view_proj);
  void add_wireframe_sphere(const D3DXMATRIX& world, const D3DXCOLOR& color, const D3DXMATRIX& view_proj);

  void  add_line(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXCOLOR& color, const D3DXMATRIX& view_proj);
  void  add_text(const D3DXVECTOR3& pos, const D3DXMATRIX& view, const D3DXMATRIX& proj, const bool billboard, const char* format, ...);

  void  add_debug_string(const char* format, ...);

private:
  struct DrawCall
  {
    DrawCall(const uint32_t vertex_count, const uint32_t start_vertex, const D3DXMATRIX& view_proj) 
      : vertex_count(vertex_count), start_vertex_location(start_vertex), view_proj(view_proj) {}
    uint32_t  vertex_count;
    uint32_t  start_vertex_location;
    D3DXMATRIX  view_proj;
  };

	typedef std::vector<DrawCall> DrawCalls;
	typedef std::map< D3D11_PRIMITIVE_TOPOLOGY, DrawCalls > DrawCallsByTopology;

	struct VertexFormatData
	{
		VertexFormatData()
			: data_(NULL)
			, vertex_size_(0)
			, num_verts(0)
			, buffer_size_(0)
			, data_ofs_(0)
			, input_layout_(NULL)
			, vertex_count_(0)
		{
		}
		uint32_t vertex_size_;
		uint32_t num_verts;
		uint32_t buffer_size_;
		uint32_t data_ofs_;
		uint8_t* data_;
		uint32_t vertex_count_;
		std::string technique_name_;

		DrawCallsByTopology draw_calls_by_topology_;

		ID3D11BufferPtr _vertex_buffer;
		ID3D11InputLayoutPtr input_layout_;
	};


	DebugRenderer();
	~DebugRenderer();

  void draw_line(const float* proj, float x1,float y1,float z1, float x2,float y2, float z2);

  bool init_vertex_buffers();
  void init_unit_sphere();
  void  color_sphere(const D3DXCOLOR& col);

  typedef std::map< uint32_t, VertexFormatData > VertexFormats;
  VertexFormats vertex_formats_;

/*
  EffectWrapper* effect_;

  ID3D11BlendStatePtr blend_state_;
  ID3D11DepthStencilStatePtr  depth_stencil_state_;

  ID3DX10FontPtr  font_;
*/

  std::vector<PosCol> sphere_verts_;

  VectorFont* vector_font_;

  std::vector< string2 >  debug_text_;

	typedef std::vector<DebugRenderDelegate> DebugRenderDelegates;
	typedef std::vector<DebugCameraDelegate> DebugCameraDelegates;

	DebugRenderDelegates _debug_render_delegates;
	DebugCameraDelegates _debug_camera_delegates;

	static DebugRenderer *_instance;
};


#endif
