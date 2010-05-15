#ifndef _RESOURCE_MANAGER_HPP
#define _RESOURCE_MANAGER_HPP

#include <hash_map>
#include "fast_delegate_bind.hpp"
#include "parser/state_parser_symbols.hpp"

class EffectWrapper;
class Scene;
struct MaterialFile;


class ResourceManager
{
public:
	static ResourceManager& instance();

	typedef stdext::hash_map< std::string, CComPtr<ID3D11BlendState> > BlendStates;
	typedef stdext::hash_map< std::string, CComPtr<ID3D11SamplerState> > SamplerStates;

	struct EffectStates
	{
		BlendStates blend_states;
		SamplerStates sampler_states;
	};

	// callbacks
  typedef fastdelegate::FastDelegate1<const EffectStates& > fnStateLoaded;
	typedef fastdelegate::FastDelegate1<EffectWrapper*> fnEffectLoaded;
	typedef fastdelegate::FastDelegate1<Scene* > fnSceneLoaded;
	typedef fastdelegate::FastDelegate1<const MaterialFile&> fnMaterialsLoaded;

	// All the loading functions are synchronous, but the given callback can be
	// called multiple times if the specified file is modified
	bool load_effect_states(const char* filename, const fnStateLoaded& fn);
	bool load_vertex_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn);
	bool load_pixel_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn);

  bool  load_shaders(const char *filename, const char *vs, const char *ps, const fnEffectLoaded& fn);

	bool load_scene(const char* filename, const fnSceneLoaded& fn);
	bool load_materials(const char* filename, const fnMaterialsLoaded& fn);

private:
	DISALLOW_COPY_AND_ASSIGN(ResourceManager);

  enum ShaderType {
    kVertexShader = 1 << 0,
    kPixelShader = 1 << 1,
  };

	// The inner functions are called both from the "file changed" signal, and directly from the load_xxx methods
	bool reload_effect_states(const char* filename);
	bool reload_vs(const std::string& filename);
	bool reload_ps(const std::string& filename);
  bool reload_vs_ps(const std::string& filename);
	bool reload_shader(const char* filename, const int shaders);
	bool reload_material(const char* filename);
	bool reload_scene(const char* filename);

	ResourceManager();
	~ResourceManager();
	static ResourceManager* _instance;


	// Structures to keep track of which user callbacks are associated with which files
	struct ShaderCallbackData
	{
    typedef const std::string& StringRef;
		ShaderCallbackData(StringRef filename, StringRef vs_entry_point, StringRef ps_entry_point, const fnEffectLoaded& fn) 
      : _filename(filename), _vs_entry_point(vs_entry_point), _ps_entry_point(ps_entry_point), _effect_loaded(fn) {}
		std::string _filename;
    std::string _vs_entry_point;
    std::string _ps_entry_point;
		fnEffectLoaded _effect_loaded;
	};

	typedef std::string Filename;
	typedef std::map< std::pair<Filename, int>, std::vector<ShaderCallbackData> > ShaderCallbacks;
	typedef stdext::hash_map< Filename, std::vector<fnStateLoaded> > StateCallbacks;
	typedef stdext::hash_map< Filename, std::vector<fnSceneLoaded> > SceneCallbacks;
	typedef stdext::hash_map< Filename, std::vector<fnMaterialsLoaded> > MaterialCallbacks;

	ShaderCallbacks _shader_callbacks;
	StateCallbacks _state_callbacks;
	SceneCallbacks _scene_callbacks;
	MaterialCallbacks _material_callbacks;
};

#endif
