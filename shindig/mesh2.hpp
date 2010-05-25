#pragma once

// Simple mesh format.
// Verts are just pos/normal
class Mesh2
{
public:

  void render(ID3D11DeviceContext *context);

private:
  CComPtr<ID3D11Buffer> _vb;
  CComPtr<ID3D11Buffer> _ib;
};
