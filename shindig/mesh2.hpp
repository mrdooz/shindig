#pragma once

// Simple mesh format.
// Verts are just pos/normal
class Mesh2
{
public:
  Mesh2();

  void render(ID3D11DeviceContext *context);

  CComPtr<ID3D11Buffer> _vb;
  CComPtr<ID3D11Buffer> _ib;

  // the layout is the connection between the vb and the effect, so
  // as long as the vb is in the mesh class, the layout might as well be here
  CComPtr<ID3D11InputLayout> _layout;

  DXGI_FORMAT _ib_format;
  int _stride;
  int _vertex_count;
  int _vertex_size;
  int _index_count;
};
