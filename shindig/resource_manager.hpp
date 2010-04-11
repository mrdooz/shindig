#ifndef _RESOURCE_MANAGER_HPP
#define _RESOURCE_MANAGER_HPP

#include <hash_map>
#include "fast_delegate_bind.hpp"
#include "parser/state_parser_symbols.hpp"

class EffectWrapper;

class ResourceManager
{
public:
	static ResourceManager& instance();

	typedef stdext::hash_map< std::string, CComPtr<ID3D11BlendState> > BlendStates;
  typedef fastdelegate::FastDelegate1<const BlendStates& > fnStateLoaded;
	typedef fastdelegate::FastDelegate1<EffectWrapper*> fnEffectLoaded;

	// All the loading functions are synchronous, but the given callback can be
	// called multiple times if the specified file is modified
	bool load_effect_states(const char* filename, const fnStateLoaded& fn);
	bool load_vertex_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn);
	bool load_pixel_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn);

private:
	DISALLOW_COPY_AND_ASSIGN(ResourceManager);

	bool reload_effect_states(const std::string& filename);
	bool reload_shader(const char* filename, const bool vertex_shader);

	ResourceManager();
	~ResourceManager();
	static ResourceManager* _instance;

	typedef std::string Filename;
	typedef std::string ShaderName;

	// workaround for C4503, 'identifier' : decorated name length exceeded, name was truncated
	struct Callbacks { typedef std::vector<fnStateLoaded> E; E e; };
	typedef stdext::hash_map< Filename, Callbacks > StateCallbacks;

	struct CallbackData
	{
		CallbackData(const std::string& filename, const std::string& entry_point, const fnEffectLoaded& fn) 
			: _filename(filename), _entry_point(entry_point), _effect_loaded(fn) {}
		std::string _filename;
		std::string _entry_point;
		fnEffectLoaded _effect_loaded;
	};
	//typedef std::vector< std::pair< ShaderName, fnEffectLoaded> > NameAndCallbacks;
	typedef stdext::hash_map< Filename, std::vector<CallbackData> > ShaderCallbacks;

	ShaderCallbacks _shader_callbacks;
	StateCallbacks _state_callbacks;
	BlendStates _blend_states;

};

#endif
