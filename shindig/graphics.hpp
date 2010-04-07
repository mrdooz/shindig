#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

class Graphics
{
public:

	static Graphics& instance();

	bool init(const HWND hwnd, const int width, const int height);
	bool close();
	void tick();
	void resize(const int width, const int height);

	ID3D11Device* device() { return _device; }
  ID3D11DeviceContext* context() { return _immediate_context; }

private:
	DISALLOW_COPY_AND_ASSIGN(Graphics);

	Graphics();
	~Graphics();

	bool init_directx(const HWND hwnd, const int width, const int height);
	bool close_directx();

	void file_changed(const std::string& filename);

	static Graphics* _instance;

	DXGI_FORMAT _buffer_format;
	CComPtr<ID3D11Device> _device;
	CComPtr<IDXGISwapChain> _swap_chain;
	CComPtr<ID3D11DeviceContext> _immediate_context;
	CComPtr<ID3D11RenderTargetView> _render_target_view;
	boost::signals2::connection _con;

};

#endif
