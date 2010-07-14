#include "stdafx.h"
#include "mesh2.hpp"

Mesh2::Mesh2()
  : _stride(0)
  , _vertex_count(0)
  , _vertex_size(0)
  , _index_count(0)
  , _ib_format(DXGI_FORMAT_UNKNOWN)
{
}

Mesh2::~Mesh2()
{

}

void Mesh2::render(ID3D11DeviceContext *context)
{
  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  set_vb(context, _vb, _stride);
  context->IASetIndexBuffer(_ib, _ib_format, 0);
  context->DrawIndexed(_index_count, 0, 0);
}
