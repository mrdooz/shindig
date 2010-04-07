#ifndef EFFECT_WRAPPER_HPP
#define EFFECT_WRAPPER_HPP

#include <string>
#include <D3DX10math.h>
#include <d3d11.h>
#include <d3dx11effect.h>
#include <hash_map>

class EffectWrapper
{
public:
  EffectWrapper();
  ~EffectWrapper();
  bool	load(const char* filename, const char* entry_point);

  bool  set_variable(const std::string& name, const float value);
  bool  set_variable(const std::string& name, const D3DXVECTOR2& value);
  bool  set_variable(const std::string& name, const D3DXVECTOR3& value);
  bool  set_variable(const std::string& name, const D3DXVECTOR4& value);
  bool  set_variable(const std::string& name, const D3DXMATRIX& value);

  bool  get_variable(ID3DX11EffectScalarVariable*& var, const std::string& name);
  bool  get_variable(ID3DX11EffectVectorVariable*& var, const std::string& name);
  bool  get_variable(ID3DX11EffectMatrixVariable*& var, const std::string& name);
  bool  get_cbuffer(ID3D11Buffer*& buffer, const std::string& name);
  bool  get_resource(ID3DX11EffectShaderResourceVariable*& resource, const std::string& name);
  bool  set_resource(const std::string& name, ID3D11ShaderResourceView* resource);

private:

	void	do_reflection(ID3DBlob* blob_out);

  void collect_variables(const D3DX11_EFFECT_DESC& desc);
  void collect_techniques(const D3DX11_EFFECT_DESC& desc);
  void collect_cbuffers(const D3DX11_EFFECT_DESC& desc);

  typedef stdext::hash_map< std::string, ID3DX11EffectTechnique* > Techniques;
  typedef stdext::hash_map< std::string, ID3DX11EffectScalarVariable*> ScalarVariables;
  typedef stdext::hash_map< std::string, ID3DX11EffectVectorVariable*> VectorVariables;
  typedef stdext::hash_map< std::string, ID3DX11EffectMatrixVariable*> MatrixVariables;
  typedef stdext::hash_map< std::string, ID3DX11EffectShaderResourceVariable*> ShaderResourceVariables;

  typedef stdext::hash_map< std::string, ID3D11Buffer* > ConstantBuffers;

  std::string _filename;

	CComPtr<ID3DX11Effect> _effect;

  Techniques techniques_;
  ScalarVariables scalar_variables_;
  VectorVariables vector_variables_;
  MatrixVariables matrix_variables_;
  ConstantBuffers constant_buffers_;
  ShaderResourceVariables shader_resource_variables_;
};

#endif
