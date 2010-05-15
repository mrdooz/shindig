#pragma once

// Helper class for dynamic vertex buffers
template<class Vtx>
class DynamicVb
{
public:
  DynamicVb()
    : _mapped(false)
  {

  }

  ~DynamicVb()
  {

  }

  bool create(int max_verts)
  {
    if (FAILED(create_dynamic_vertex_buffer(Graphics::instance().device(), max_verts, sizeof(Vtx), &_vb)))
      return false;

    return true;
  }

  Vtx* map()
  {
    ID3D11DeviceContext *c = Graphics::instance().context();
    D3D11_MAPPED_SUBRESOURCE r;
    if (FAILED(c->Map(_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &r)))
      return NULL;

    _mapped = true;
    return (Vtx*)r.pData;
  }

  void unmap()
  {
    assert(_mapped);
    ID3D11DeviceContext *c = Graphics::instance().context();
    c->Unmap(_vb, 0);
    _mapped = false;
  }

  ID3D11Buffer *vb() { return _vb; }

private:
  bool _mapped;
  CComPtr<ID3D11Buffer> _vb;
};
