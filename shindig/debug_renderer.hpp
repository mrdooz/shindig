#ifndef DEBUG_RENDERER_HPP
#define DEBUG_RENDERER_HPP

#include <celsus/math_utils.hpp>

#include "vector_font.hpp"
#include "bounding.hpp"

class EffectWrapper;

typedef CComPtr<ID3D11Buffer> ID3D11BufferPtr;
typedef CComPtr<ID3D11InputLayout> ID3D11InputLayoutPtr;
typedef CComPtr<ID3D11Device> ID3D11DevicePtr;


// objects that want to be debug drawn can implement this
struct DebugDraw
{
  string2 name;
  D3DXMATRIX orientation;

  uint32_t bounding_flags;
  Sphere sphere;
};

struct DebugCamera
{
	Frame frame;
	float fov;
	float near_plane, far_plane;
};

typedef fastdelegate::FastDelegate1<DebugDraw *> DebugRenderDelegate;
typedef fastdelegate::FastDelegate1<DebugCamera *> DebugCameraDelegate;

class FontWriter;

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
  bool close();
  void start_frame();
  void end_frame();
  // vertex_format is a bitmask made from Pos, Normal, Color
  void add_verts(const uint32_t vertex_format, const D3D11_PRIMITIVE_TOPOLOGY topology, const float* verts, const uint32_t vertex_count, 
    const D3DXMATRIX& world, const D3DXMATRIX& view_proj);
  void render();

	// helper functions to draw debug info in the scene's space
	// the calls are enclosed in begin/end blocks so we don't have to worry about the
	// scene rendering to strange render targets etc
	void begin_debug_draw();
  void add_wireframe_sphere(const D3DXVECTOR3& center, const float radius, const D3DXCOLOR& color, const D3DXMATRIX& world, const D3DXMATRIX& view_proj);
  void add_wireframe_sphere(const D3DXMATRIX& world, const D3DXCOLOR& color, const D3DXMATRIX& view_proj);

  void add_line(const D3DXVECTOR3& a, const D3DXVECTOR3& b, const D3DXCOLOR& color, const D3DXMATRIX& view_proj);
  void add_text(const D3DXVECTOR3& pos, const D3DXMATRIX& view, const D3DXMATRIX& proj, const bool billboard, const char* format, ...);
  void add_debug_string(const char* format, ...);
	void end_debug_draw();

	bool enabled() const { return _enabled; }
	void set_enabled(bool value) { _enabled = value; }

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
			: _data(NULL)
			, vertex_size_(0)
			, num_verts(0)
			, buffer_size_(0)
			, _data_ofs(0)
			//, input_layout_(NULL)
			, vertex_count_(0)
		{
		}
		uint32_t vertex_size_;
		uint32_t num_verts;
		uint32_t buffer_size_;
		uint32_t _data_ofs;
		uint8_t* _data;
		uint32_t vertex_count_;

		DrawCallsByTopology draw_calls_by_topology_;

		CComPtr<ID3D11Buffer> _vertex_buffer;
		//CComPtr<ID3D11InputLayoutPtr> input_layout_;
	};


	DebugRenderer();
	~DebugRenderer();

	void load_effect(EffectWrapper *effect);
	bool load_states(const string2& filename);

  void draw_line(const float* proj, float x1,float y1,float z1, float x2,float y2, float z2);

  bool init_vertex_buffers();
  void  color_sphere(const D3DXCOLOR& col);

  typedef std::map< uint32_t, VertexFormatData > VertexFormats;
  VertexFormats vertex_formats_;

	CComPtr<ID3D11InputLayout> _layout;
	CComPtr<ID3D11DepthStencilState> _dss;
	CComPtr<ID3D11BlendState> _blend_state;
	std::auto_ptr<EffectWrapper> _effect;

  std::vector<PosCol> _sphere_verts;
  DynamicVb<PosCol> _verts;

  VectorFont* _vector_font;

  std::vector< string2 >  debug_text_;

	typedef std::vector<DebugRenderDelegate> DebugRenderDelegates;
	typedef std::vector<DebugCameraDelegate> DebugCameraDelegates;

	DebugRenderDelegates _debug_render_delegates;
	DebugCameraDelegates _debug_camera_delegates;

	bool _enabled;

	FontWriter *_font_writer;
	static DebugRenderer *_instance;
};


#endif
