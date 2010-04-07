#include "stdafx.h"
#include "effect_wrapper.hpp"
#include "graphics.hpp"

bool tester(bool value, const char* str)
{
  return value;
}

bool tester(HRESULT hr, const char* str)
{
  return SUCCEEDED(hr);
}

#define LOGGED_ERR_BOOL(x) if (!tester(x, #x)) {	DebugBreak(); return false; } 


EffectWrapper::EffectWrapper()
{
}

EffectWrapper::~EffectWrapper()
{
  map_delete(_buffer_variables);
  map_delete(_constant_buffers);
}


bool EffectWrapper::load(const char* filename, const char* entry_point)
{

	uint8_t* buf = NULL;
	uint32_t len = 0;
	load_file(buf, len, filename);

	ID3DBlob* error_blob = NULL;
	ID3DBlob* blob_out = NULL;

	LOGGED_ERR_BOOL(D3DCompile(buf, len, "none", NULL, NULL, entry_point, "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &blob_out, &error_blob));
	int t = blob_out->GetBufferSize();
	ID3D11VertexShader* shader = NULL;
	Graphics::instance().device()->CreateVertexShader(blob_out->GetBufferPointer(), blob_out->GetBufferSize(), NULL, &shader);

	do_reflection(blob_out);

  _filename = filename;
  //g_system->watch_file(filename, fastdelegate::bind(&EffectWrapper::file_changed, this));
  return true;
}


void EffectWrapper::do_reflection(ID3DBlob* blob_out)
{
	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect(blob_out->GetBufferPointer(), blob_out->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
	D3D11_SHADER_DESC shader_desc;
	pReflector->GetDesc(&shader_desc);

	static D3D_FEATURE_LEVEL levels[] = 
	{
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	for (int i = 0; i < ELEMS_IN_ARRAY(levels); ++i) {
		HRESULT hr = pReflector->GetMinFeatureLevel(&levels[i]);
		int a = 10;
	}

	D3D11_SHADER_BUFFER_DESC d;
	D3D11_SHADER_VARIABLE_DESC vd;
	D3D11_SHADER_TYPE_DESC td;

	// global constant buffer is called "$Globals"
	for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i) {
		ID3D11ShaderReflectionConstantBuffer* rcb = pReflector->GetConstantBufferByIndex(i);
		// create constant buffer
		rcb->GetDesc(&d);
		CD3D11_BUFFER_DESC bb(d.Size, D3D11_BIND_CONSTANT_BUFFER);
		ID3D11Buffer *cb = NULL;
		HRESULT hr = Graphics::instance().device()->CreateBuffer(&bb, NULL, &cb);
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
