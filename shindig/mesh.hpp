#ifndef _MESH_HPP_
#define _MESH_HPP_

#include <D3DX10math.h>

struct Mesh
{
	Mesh(const std::string& name) : _name(name) {}
	std::string _name;
	std::string _transform_name;
	std::vector<D3D11_INPUT_ELEMENT_DESC> _input_element_descs;

	DXGI_FORMAT _index_buffer_format;

	int32_t _index_count;
	int32_t _vertex_buffer_stride;
	D3DXVECTOR3 _bounding_sphere_center;
	float _bounding_sphere_radius;

	CComPtr<ID3D11Buffer> _vertex_buffer; 
	CComPtr<ID3D11Buffer> _index_buffer;
};


#endif
