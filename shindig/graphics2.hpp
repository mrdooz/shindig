#ifndef _GRAPHICS2_HPP_
#define _GRAPHICS2_HPP_

#include <celsus/celsus.hpp>
#include <d3d11.h>
#include <D3DX10math.h>
#include <stdint.h>
#include <atlbase.h>

typedef uint64_t uint64;
typedef uint32_t uint32;

class Graphics2
{
public:

	enum RenderCommand {
		kClearBuffer,
	};

	struct CommandKey {
		// taken from http://realtimecollisiondetection.net/blog/?p=86
		uint32 layer : 2;
		//uint32 viewport : 3;
		//uint32 viewport_layer : 3;
		uint32 translucency : 2;
		uint32 is_cmd : 1;
		union {
			struct {
				RenderCommand command;
				uint32 cmd_seq : 21;
			};
			struct {
				uint32 material_id : 29;
				uint32 depth : 24;
			};
		};
	};

	struct RenderData {
		uint32 vb_idx;
		uint32 ib_idx;
		uint32 prim_count;
	};


	static Graphics2& instance();

	bool init_directx(const HWND hwnd, const int width, const int height);
	bool close();
  void	clear(const D3DXCOLOR& c);
  void	clear();
  void  set_clear_color(const D3DXCOLOR& c) { _clear_color = c; }
	void	present();
  void  tick();
	void resize(const int width, const int height);

	void add_draw_call(uint32 layer, uint32 translucency, uint32 depth, uint32 material_id, const RenderData &data);
	void add_command(uint32 layer, RenderCommand command, uint32 seq);

	void process_commands();

	ID3D11Device* device() { return _device; }
  ID3D11DeviceContext* context() { return _immediate_context; }

  const D3D11_VIEWPORT& viewport() const { return _viewport; }

	void set_default_render_target();

  D3D_FEATURE_LEVEL feature_level() const { return _feature_level; }

	bool create_render_target(int width, int height, RenderTargetData *out);

  ID3D11RasterizerState *default_rasterizer_state() const { return _default_rasterizer_state; }
  ID3D11DepthStencilState *default_depth_stencil_state() const { return _default_depth_stencil_state; }
  uint32_t default_stencil_ref() const { return 0; }
  ID3D11BlendState *default_blend_state() const { return _default_blend_state; }
  const float *default_blend_factors() const { return _default_blend_factors; }
  uint32_t default_sample_mask() const { return 0xffffffff; }

  CComPtr<IDXGISwapChain>& swap_chain() { return _swap_chain; }

  float fps() const { return _fps; }
  int width() const { return _width; }
  int height() const { return _height; }

private:
	DISALLOW_COPY_AND_ASSIGN(Graphics2);

	Graphics2();
	~Graphics2();

  bool create_back_buffers(int width, int height);
	static Graphics2* _instance;
  static bool _deleted;

	int _width;
	int _height;
	D3D11_VIEWPORT _viewport;
	DXGI_FORMAT _buffer_format;
  D3D_FEATURE_LEVEL _feature_level;
	CComPtr<ID3D11Device> _device;
	CComPtr<IDXGISwapChain> _swap_chain;
	CComPtr<ID3D11DeviceContext> _immediate_context;
	CComPtr<ID3D11RenderTargetView> _render_target_view;
	CComPtr<ID3D11Texture2D> _depth_stencil;
	CComPtr<ID3D11DepthStencilView> _depth_stencil_view;

	CComPtr<ID3D11Texture2D> _back_buffer;
	CComPtr<ID3D11ShaderResourceView> _shared_texture_view;
	CComPtr<ID3D11Texture2D> _shared_texture;
	CComPtr<IDXGIKeyedMutex> _keyed_mutex_10;
	CComPtr<IDXGIKeyedMutex> _keyed_mutex_11;

  CComPtr<ID3D11Debug> _d3d_debug;

  CComPtr<ID3D11RasterizerState> _default_rasterizer_state;
  CComPtr<ID3D11DepthStencilState> _default_depth_stencil_state;
  CComPtr<ID3D11SamplerState> _default_sampler_state;
  float _default_blend_factors[4];
  CComPtr<ID3D11BlendState> _default_blend_state;

  D3DXCOLOR _clear_color;
  DWORD _start_fps_time;
  int32_t _frame_count;
  float _fps;
};

#endif
