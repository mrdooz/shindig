#include "stdafx.h"
#include "effect_wrapper.hpp"
#include <celsus/CelsusExtra.hpp>
#include <D3Dcompiler.h>
#include <D3dx11effect.h>
#include <boost/scoped_array.hpp>

#include <crtdefs.h>

ID3DX11Effect* load_effect(const char* filename, ID3D11Device* device)
{

	uint32_t len = 0;
	boost::scoped_array<uint8_t> hlsl(load_file(filename, true, &len));
	if (!hlsl) {
		LOG_WARNING_LN("Error loading file: %s", filename);
		return NULL;
	}

	ID3D10Blob* shader;
	HRESULT hr;
	D3DX11CompileFromFile(filename, NULL, NULL, "vsMain", "vs_4_0", 0, 0, NULL, &shader, NULL, &hr);
	ID3D11VertexShader* vs = NULL;
	if (FAILED(device->CreateVertexShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &vs))) {
		return NULL;
	}
/*
	ID3DX11Effect* d3d_effect = NULL;
	if (FAILED(D3DX11CreateEffectFromMemory(shader->GetBufferPointer(), shader->GetBufferSize(), 0, device, &d3d_effect))) {
			return NULL;
	}
	*/
	//return d3d_effect;
	return NULL;
}


EffectWrapper::EffectWrapper(const CComPtr<ID3D11Device>& device)
  : _device(device)
{
}

EffectWrapper::~EffectWrapper()
{
  //g_system->unwatch_file(_filename.c_str(), fastdelegate::bind(&EffectWrapper::file_changed, this));

}

bool EffectWrapper::load(const char* filename)
{

	uint8_t* buf = NULL;
	uint32_t len = 0;
	load_file(buf, len, filename);

	ID3DBlob* error_blob = NULL;
	ID3DBlob* blob_out = NULL;

	ID3DX11Effect* e = NULL;

	HRESULT hr = D3DCompile(buf, len, "none", NULL, NULL, "vsMain", "vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &blob_out, &error_blob);
	int t = blob_out->GetBufferSize();
	ID3D11VertexShader* shader = NULL;
	_device->CreateVertexShader(blob_out->GetBufferPointer(), blob_out->GetBufferSize(), NULL, &shader);

	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect(blob_out->GetBufferPointer(), blob_out->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pReflector);
	D3D11_SHADER_DESC shader_desc;
	pReflector->GetDesc(&shader_desc);

	D3D_FEATURE_LEVEL levels[] = 
	{
		D3D_FEATURE_LEVEL_9_1,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL feature_level;
	for (int i = 0; i < ELEMS_IN_ARRAY(levels); ++i) {
		hr = pReflector->GetMinFeatureLevel(&levels[i]);
		int a = 10;
	}

	D3D11_DEPTH_STENCIL_DESC d;

	for (UINT i = 0; i < shader_desc.ConstantBuffers; ++i) {
		ID3D11ShaderReflectionConstantBuffer* b = pReflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC d;
		b->GetDesc(&d);
		for (UINT j = 0; j < d.Variables; ++j) {
			ID3D11ShaderReflectionVariable* v = b->GetVariableByIndex(j);
				D3D11_SHADER_VARIABLE_DESC vd;
				ID3D11ShaderReflectionType* t = v->GetType();
				v->GetDesc(&vd);
				D3D11_SHADER_TYPE_DESC td;
				t->GetDesc(&td);
				int a = 10;

		}
	}


  if (!load_inner(filename)) {
    return false;
  }

  _filename = filename;
  //g_system->watch_file(filename, fastdelegate::bind(&EffectWrapper::file_changed, this));
  return true;
}

void EffectWrapper::file_changed()
{
  if (!load_inner(_filename.c_str())) {
    LOG_WARNING_LN("Error reloading shader: %s", _filename.c_str());
  }
}

bool EffectWrapper::load_inner(const char* filename)
{

  ID3DX11Effect* effect = load_effect(filename, _device);
  if (effect == NULL) {
    return false;
  }

  techniques_.clear();
  scalar_variables_.clear();
  vector_variables_.clear();
  matrix_variables_.clear();
  constant_buffers_.clear();
  shader_resource_variables_.clear();


  _effect.Attach(effect);
  D3DX11_EFFECT_DESC desc;
  _effect->GetDesc(&desc);
  collect_variables(desc);
  collect_techniques(desc);
  collect_cbuffers(desc);

  return true;
}
/*
bool EffectWrapper::get_pass_desc(D3D11_PASS_DESC& desc, const std::string& technique_name, const uint32_t pass)
{
  Techniques::iterator it = techniques_.find(technique_name);
  if (it != techniques_.end()) {
    return SUCCEEDED(it->second->GetPassByIndex(0)->GetDesc(&desc));
  }
  LOG_WARNING_LN("Unknown technique: %s", technique_name.c_str());
  return false;
}
*/
bool EffectWrapper::get_technique(ID3DX11EffectTechnique*& technique, const std::string& name)
{
  Techniques::iterator it = techniques_.find(name);
  if (it != techniques_.end()) {
    technique = it->second;
    return true;
  }
  technique = NULL;
  return false;
}

bool EffectWrapper::set_technique(const std::string& technique_name)
{
  Techniques::iterator it = techniques_.find(technique_name);
  if (it != techniques_.end()) {
    return SUCCEEDED(it->second->GetPassByIndex(0)->Apply(0, NULL));
  }
  LOG_WARNING_LN_ONESHOT("[%s] Unknown technique: %s", __FUNCTION__, technique_name.c_str());
  return false;
}

bool EffectWrapper::set_variable(const std::string& name, const float value)
{
  ScalarVariables::iterator it = scalar_variables_.find(name);
  if (it != scalar_variables_.end()) {
    if (SUCCEEDED(it->second->SetFloat(value))) {
      return true;
    }
  }
  return false;
}

bool EffectWrapper::set_variable(const std::string& name, const D3DXVECTOR2& value)
{
  return set_variable(name, D3DXVECTOR4(value.x, value.y, 0, 0));
}

bool EffectWrapper::set_variable(const std::string& name, const D3DXVECTOR3& value)
{
  return set_variable(name, D3DXVECTOR4(value.x, value.y, value.z, 0));
}

bool EffectWrapper::set_variable(const std::string& name, const D3DXVECTOR4& value)
{
  VectorVariables::iterator it = vector_variables_.find(name);
  if (it != vector_variables_.end()) {
    if (SUCCEEDED(it->second->SetFloatVector((float*)&value[0]))) {
      return true;
    }
  }
  return false;
}

bool EffectWrapper::set_variable(const std::string& name, const D3DXMATRIX& value)
{
  MatrixVariables::iterator it = matrix_variables_.find(name);
  if (it != matrix_variables_.end()) {
    if (SUCCEEDED(it->second->SetMatrix((float*)&value[0]))) {
      return true;
    }
  }
  return false;
}

bool EffectWrapper::set_resource(const std::string& name, ID3D11ShaderResourceView* resource)
{
  ShaderResourceVariables::iterator it = shader_resource_variables_.find(name);
  if (it != shader_resource_variables_.end()) {
    if (SUCCEEDED(it->second->SetResource(resource))) {
      return true;
    }
  }
  return false;
}

bool EffectWrapper::get_resource(ID3DX11EffectShaderResourceVariable*& resource, const std::string& name)
{
  ShaderResourceVariables::iterator it = shader_resource_variables_.find(name);
  if (it != shader_resource_variables_.end()) {
    resource = it->second;
    return true;
  }
  return false;
}

bool EffectWrapper::get_variable(ID3DX11EffectScalarVariable*& var, const std::string& name)
{
  ScalarVariables::iterator it = scalar_variables_.find(name);
  if (it != scalar_variables_.end()) {
    var = it->second;
    return true;
  }
  return false;
}

bool EffectWrapper::get_variable(ID3DX11EffectVectorVariable*& var, const std::string& name)
{
  VectorVariables::iterator it = vector_variables_.find(name);
  if (it != vector_variables_.end()) {
    var = it->second;
    return true;
  }
  return false;
}

bool EffectWrapper::get_variable(ID3DX11EffectMatrixVariable*& var, const std::string& name)
{
  MatrixVariables::iterator it = matrix_variables_.find(name);
  if (it != matrix_variables_.end()) {
    var = it->second;
    return true;
  }
  return false;
}

bool EffectWrapper::get_cbuffer(ID3D11Buffer*& buffer, const std::string& name)
{
  ConstantBuffers::iterator it = constant_buffers_.find(name);
  if (it != constant_buffers_.end()) {
    buffer = it->second;
    return true;
  }
  return false;
}

void EffectWrapper::collect_variables(const D3DX11_EFFECT_DESC& desc)
{
  for (uint32_t i = 0; i < desc.GlobalVariables; ++i) {

    ID3DX11EffectVariable* var = _effect->GetVariableByIndex(i);
    ID3DX11EffectType* type = var->GetType();
    D3DX11_EFFECT_TYPE_DESC type_desc;
    D3DX11_EFFECT_VARIABLE_DESC var_desc;
    type->GetDesc(&type_desc);
    var->GetDesc(&var_desc);
    const char* type_name = type_desc.TypeName;
    const char* var_name = var_desc.Name;

    ID3DX11EffectVariable* effect_variable = _effect->GetVariableByName(var_name);
    if (effect_variable == NULL) {
      LOG_WARNING_LN("Error getting variable: %s", var_name);
      continue;
    }

    if (strcmp(type_name, "float") == 0) {
      if (ID3DX11EffectScalarVariable* typed_variable = effect_variable->AsScalar()) {
        scalar_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else if (strcmp(type_name, "float2") == 0) {
      if (ID3DX11EffectVectorVariable* typed_variable = effect_variable->AsVector()) {
        vector_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else if (strcmp(type_name, "float3") == 0) {
      if (ID3DX11EffectVectorVariable* typed_variable = effect_variable->AsVector()) {
        vector_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else if (strcmp(type_name, "float4") == 0) {
      if (ID3DX11EffectVectorVariable* typed_variable = effect_variable->AsVector()) {
        vector_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else if (strcmp(type_name, "int2") == 0) {
      if (ID3DX11EffectVectorVariable* typed_variable = effect_variable->AsVector()) {
        vector_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else if (strcmp(type_name, "float4x4") == 0) {
      if (ID3DX11EffectMatrixVariable* typed_variable = effect_variable->AsMatrix()) {
        matrix_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else if (strcmp(type_name, "Texture2D") == 0) {
      if (ID3DX11EffectShaderResourceVariable* typed_variable = effect_variable->AsShaderResource()) {
        shader_resource_variables_.insert(std::make_pair(var_name, typed_variable));
      }
    } else {
      LOG_WARNING_LN("Unknown variable: %s, type: %s", var_desc.Name, type_name);
    }
  }
}

void EffectWrapper::collect_techniques(const D3DX11_EFFECT_DESC& desc)
{
  for (uint32_t i = 0; i < desc.Techniques; ++i ) {
    ID3DX11EffectTechnique* technique = _effect->GetTechniqueByIndex(i);
    D3DX11_TECHNIQUE_DESC technique_desc;
    technique->GetDesc(&technique_desc);
    techniques_.insert(std::make_pair(technique_desc.Name, technique));
  }
}

void EffectWrapper::collect_cbuffers(const D3DX11_EFFECT_DESC& desc)
{
  for (uint32_t i = 0; i < desc.ConstantBuffers; ++i) {
    ID3DX11EffectConstantBuffer* cbuffer = _effect->GetConstantBufferByIndex(i);
    D3DX11_EFFECT_VARIABLE_DESC var_desc;
    cbuffer->GetDesc(&var_desc);
    ID3D11Buffer* buf = NULL;
    cbuffer->GetConstantBuffer(&buf);
    constant_buffers_.insert(std::make_pair(var_desc.Name, buf));
  }
}
