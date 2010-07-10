#pragma once

// Helper class for dynamic vertex buffers
template<class Vtx>
class DynamicVb
{
public:

  enum { stride = sizeof(Vtx) };
  DynamicVb()
    : _mapped(false)
    , _org(nullptr)
  {
  }

  ~DynamicVb()
  {
  }

  bool create(int max_verts)
  {
    return SUCCEEDED(create_dynamic_vertex_buffer(Graphics::instance().device(), max_verts, sizeof(Vtx), &_vb));
  }

  Vtx* map()
  {
    ID3D11DeviceContext *c = Graphics::instance().context();
    D3D11_MAPPED_SUBRESOURCE r;
    if (FAILED(c->Map(_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &r)))
      return NULL;

    _mapped = true;
    _org = (Vtx*)r.pData;
    return _org;
  }

  int unmap(Vtx *final = NULL)
  {
    assert(_mapped);
    if (!_mapped) return -1;
    ID3D11DeviceContext *c = Graphics::instance().context();
    c->Unmap(_vb, 0);
    _mapped = false;

    int res = 0;
    // calc # verts inserted
    if (final != NULL)
      res = final - _org;
    return res;

  }

  ID3D11Buffer *vb() { return _vb; }

private:
  Vtx *_org;
  bool _mapped;
  CComPtr<ID3D11Buffer> _vb;
};
