#ifndef _SYSTEM_HPP_
#define _SYSTEM_HPP_

class System
{
public:
  System();
  ~System();
  bool init_directx(const HWND hwnd, const int width, const int height);
  void tick();
  void resize(const int width, const int height);
private:

  DXGI_FORMAT _buffer_format;

  CComPtr<ID3D11Device> _device;
  CComPtr<IDXGISwapChain> _swap_chain;
  CComPtr<ID3D11DeviceContext> _immediate_context;
  CComPtr<ID3D11RenderTargetView> _render_target_view;

};

#endif
