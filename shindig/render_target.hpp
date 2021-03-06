#ifndef _RENDER_TARGET_HPP
#define _RENDER_TARGET_HPP

class RenderTarget
{
public:
	RenderTarget();
	bool	create(const int width, const int height);
	void	set();
	void	clear(const D3DXCOLOR& c);

	ID3D11RenderTargetView* render_target_view() const { return _render_target_view; }
	ID3D11DepthStencilView* depth_stencil_view() const { return _depth_stencil_view; }
	ID3D11ShaderResourceView* shader_resource_view() const { return _shader_resource_view; }
private:
	int	_width;
	int	_height;

	D3D11_VIEWPORT _viewport;
	CComPtr<ID3D11Texture2D> _render_target;
	CComPtr<ID3D11RenderTargetView> _render_target_view;
	CComPtr<ID3D11ShaderResourceView> _shader_resource_view;
	CComPtr<ID3D11Texture2D> _depth_stencil;
	CComPtr<ID3D11DepthStencilView> _depth_stencil_view;
};

#endif
