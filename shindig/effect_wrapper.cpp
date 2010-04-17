#include "stdafx.h"
#include "effect_wrapper.hpp"
#include "graphics.hpp"

EffectWrapper::EffectWrapper()
{
}

EffectWrapper::~EffectWrapper()
{
  map_delete(_buffer_variables);
  map_delete(_constant_buffers);
}

bool EffectWrapper::load_vertex_shader(const char* filename, const char* entry_point)
{
	return load_inner(filename, entry_point, true);
}

bool EffectWrapper::load_pixel_shader(const char* filename, const char* entry_point)
{
	return load_inner(filename, entry_point, false);
}

bool EffectWrapper::load_inner(const char* filename, const char* entry_point, bool vertex_shader)
{
	uint8_t* buf = NULL;
	uint32_t len = 0;
	load_file(buf, len, filename);

	ID3DBlob* error_blob = NULL;

	if (vertex_shader) {
		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &_shader_blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(Graphics::instance().device()->CreateVertexShader(_shader_blob->GetBufferPointer(), _shader_blob->GetBufferSize(), NULL, &_vertex_shader),
			ErrorPredicate<HRESULT>, LOG_ERROR_LN);

	} else {
		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &_shader_blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(Graphics::instance().device()->CreatePixelShader(_shader_blob->GetBufferPointer(), _shader_blob->GetBufferSize(), NULL, &_pixel_shader),
			ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	}

	RETURN_ON_FAIL_BOOL(do_reflection(), ErrorPredicate<bool>, LOG_ERROR_LN);

  _filename = filename;
  return true;
}


bool EffectWrapper::do_reflection()
{
	ID3D11ShaderReflection* pReflector = NULL; 
	RETURN_ON_FAIL_BOOL(D3DReflect(_shader_blob->GetBufferPointer(), _shader_blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector),
    ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	D3D11_SHADER_DESC shader_desc;
	pReflector->GetDesc(&shader_desc);

	D3D11_SHADER_BUFFER_DESC d;
	D3D11_SHADER_VARIABLE_DESC vd;
	D3D11_SHADER_TYPE_DESC td;

	// global constant buffer is called "$Globals"
	for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i) {
		ID3D11ShaderReflectionConstantBuffer* rcb = pReflector->GetConstantBufferByIndex(i);
		// create constant buffer
		rcb->GetDesc(&d);
		CD3D11_BUFFER_DESC bb(d.Size, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
		ID3D11Buffer *cb = NULL;
		RETURN_ON_FAIL_BOOL(Graphics::instance().device()->CreateBuffer(&bb, NULL, &cb), ErrorPredicate<HRESULT>, LOG_ERROR_LN);
    ConstantBuffer* cur_cb = new ConstantBuffer(d.Name, cb, bb);
    
		for (UINT j = 0; j < d.Variables; ++j) {
			ID3D11ShaderReflectionVariable* v = rcb->GetVariableByIndex(j);
			ID3D11ShaderReflectionType* t = v->GetType();
			v->GetDesc(&vd);
			t->GetDesc(&td);

      _buffer_variables.insert(std::make_pair(vd.Name, new BufferVariable(vd.Name, cur_cb, vd, td)));
		}
    _constant_buffers.insert(std::make_pair(cur_cb->_name, cur_cb ));
	}
	return true;
}


bool EffectWrapper::set_resource(const std::string& name, ID3D11ShaderResourceView* resource)
{
/*
  ShaderResourceVariables::iterator it = shader_resource_variables_.find(name);
  if (it != shader_resource_variables_.end()) {
    if (SUCCEEDED(it->second->SetResource(resource))) {
      return true;
    }
  }
*/
  return false;
}

void EffectWrapper::set_cbuffer()
{
	ID3D11DeviceContext* context = Graphics::instance().context();
	if (_vertex_shader) {
		for (ConstantBuffers::iterator i = _constant_buffers.begin(), e = _constant_buffers.end(); i != e; ++i) {
			ConstantBuffer* b = i->second;
			ID3D11Buffer* buf[1] = { b->_buffer };
			context->VSSetConstantBuffers(0, 1, buf);
		}

	} else {
		for (ConstantBuffers::iterator i = _constant_buffers.begin(), e = _constant_buffers.end(); i != e; ++i) {
			ConstantBuffer* b = i->second;
			ID3D11Buffer* buf[1] = { b->_buffer };
			context->PSSetConstantBuffers(0, 1, buf);
		}

	}
}

void EffectWrapper::unmap_buffers()
{
  ID3D11DeviceContext* context = Graphics::instance().context();
  for (ConstantBuffers::iterator i = _constant_buffers.begin(), e = _constant_buffers.end(); i != e; ++i) {
    ConstantBuffer* b = i->second;
    if (b->_mapped) {
      b->_mapped = false;
      context->Unmap(b->_buffer, 0);
    }
  }
}

ID3D11InputLayout* EffectWrapper::create_input_layout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elems)
{
	ID3D11InputLayout* layout = NULL;

	RETURN_ON_FAIL_PTR(Graphics::instance().device()->CreateInputLayout(&elems[0], elems.size(), _shader_blob->GetBufferPointer(), _shader_blob->GetBufferSize(), &layout),
		ErrorPredicate<HRESULT>, LOG_ERROR_LN);
	return layout;
}
