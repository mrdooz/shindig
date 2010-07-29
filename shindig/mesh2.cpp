#include "stdafx.h"
#include "mesh2.hpp"
#include "app.hpp"

Mesh2::Mesh2()
  : _stride(0)
  , _vertex_count(0)
  , _vertex_size(0)
  , _index_count(0)
  , _ib_format(DXGI_FORMAT_UNKNOWN)
  , _wireframe(false)
{
  App::instance().add_appstate_callback(fastdelegate::MakeDelegate(this, &Mesh2::bool_state_changed));
}

Mesh2::~Mesh2()
{

}

void Mesh2::render(ID3D11DeviceContext *context)
{
  context->IASetInputLayout(_layout);
  context->IASetPrimitiveTopology(_wireframe ? D3D11_PRIMITIVE_TOPOLOGY_LINELIST : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  set_vb(context, _vb, _stride);
  context->IASetIndexBuffer(_ib, _ib_format, 0);
  context->DrawIndexed(_index_count, 0, 0);
}

void Mesh2::bool_state_changed(const string2& name, const bool& value)
{
  _wireframe = value;
}
