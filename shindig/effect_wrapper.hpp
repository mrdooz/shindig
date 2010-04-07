#ifndef EFFECT_WRAPPER_HPP
#define EFFECT_WRAPPER_HPP

#include <string>
#include <D3DX10math.h>
#include <d3d11.h>
#include <hash_map>

class EffectWrapper
{
public:
  EffectWrapper();
  ~EffectWrapper();
  bool	load(const char* filename, const char* entry_point);

  template<typename T> bool set_variable(const std::string& name, const T& value)
  {
    // find variable
    BufferVariables::iterator it = _buffer_variables.find(name);
    if (it == _buffer_variables.end()) {
      LOG_WARNING_LN("Variable not found: %s", name.c_str());
      return false;
    }

    BufferVariable* var = it->second;
    // check the size
    if (var->_var_desc.Size != sizeof(T)) {
      LOG_WARNING_LN("Variable size doesn't match: %s", name.c_str());
      return false;
    }

    // map the buffer
    if (!var->_buffer->_mapped) {
      Graphics::instance().context()->Map(var->_buffer->_buffer, 0, D3D11_MAP_WRITE, 0, &var->_buffer->_resource);
    }

    // set
    memcpy((uint8_t*)var->_buffer->_resource.pData + var->_var_desc.StartOffset, &value, sizeof(T));

    return true;

  }
  void  unmap_buffers();

  bool  set_resource(const std::string& name, ID3D11ShaderResourceView* resource);

private:

  struct ConstantBuffer
  {
    ConstantBuffer(const std::string& name, ID3D11Buffer* buffer, const D3D11_BUFFER_DESC& desc)
      : _name(name), _mapped(false), _desc(desc) 
    {
      _buffer.Attach(buffer);
    }
    std::string _name;
    bool _mapped;
    D3D11_MAPPED_SUBRESOURCE _resource;
    CComPtr<ID3D11Buffer> _buffer;
    D3D11_BUFFER_DESC _desc;
  };

  struct BufferVariable
  {
    BufferVariable(const std::string& name, ConstantBuffer* buffer, const D3D11_SHADER_VARIABLE_DESC& vd, const D3D11_SHADER_TYPE_DESC& td)
      : _name(name), _buffer(buffer), _var_desc(vd), _type_desc(td)
    {
    }
    std::string _name;
    ConstantBuffer* _buffer;
    D3D11_SHADER_VARIABLE_DESC _var_desc;
    D3D11_SHADER_TYPE_DESC _type_desc;
  };

  typedef std::string BufferName;
  typedef std::string VariableName;
  typedef std::map< BufferName, ConstantBuffer* > ConstantBuffers;
  typedef stdext::hash_map< VariableName, BufferVariable* > BufferVariables;

	void	do_reflection(ID3DBlob* blob_out);

  std::string _filename;
  ConstantBuffers _constant_buffers;
  BufferVariables _buffer_variables;

};

#endif
