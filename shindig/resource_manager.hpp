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

	typedef std::map< string2, CComPtr<ID3D11BlendState> > BlendStates;
	typedef std::map< string2, CComPtr<ID3D11SamplerState> > SamplerStates;

	struct EffectStates
	{
		BlendStates blend_states;
		SamplerStates sampler_states;
	};

	// callbacks
  typedef fastdelegate::FastDelegate1<const EffectStates& > fnStateLoaded;
  //typedef fastdelegate::FastDelegate1<EffectWrapper*> fnEffectLoaded;
  typedef std::function< void (EffectWrapper*) > fnEffectLoaded;
	typedef fastdelegate::FastDelegate1<Scene* > fnSceneLoaded;
	typedef fastdelegate::FastDelegate1<const MaterialFile&> fnMaterialsLoaded;

	// All the loading functions are synchronous, but the given callback can be
	// called multiple times if the specified file is modified
	bool load_effect_states(const char* filename, const fnStateLoaded& fn);
	bool  load_shaders(const char *filename, const char *vs, const char *gs, const char *ps, const fnEffectLoaded& fn);

	bool load_scene(const char* filename, const fnSceneLoaded& fn);
	bool load_materials(const char* filename, const fnMaterialsLoaded& fn);

private:
	DISALLOW_COPY_AND_ASSIGN(ResourceManager);

  enum ShaderType {
    kVertexShader = 1 << 0,
		kGeometryShader = 1 << 1,
    kPixelShader = 1 << 2,
  };

	// The inner functions are called both from the "file changed" signal, and directly from the load_xxx methods
	bool reload_effect_states(const char* filename);
	bool reload_shader(const char* filename, const int shaders);
	bool reload_material(const char* filename);
	bool reload_scene(const char* filename);

	ResourceManager();
	~ResourceManager();
	static ResourceManager* _instance;


	// Structures to keep track of which user callbacks are associated with which files
	struct ShaderCallbackData
	{
    typedef const string2& StringRef;
		ShaderCallbackData(StringRef filename, StringRef vs, StringRef gs, StringRef ps, const fnEffectLoaded& fn) 
      : filename(filename), vs(vs), gs(gs), ps(ps), fn(fn) {}
		string2 filename;
    string2 vs;
		string2 gs;
    string2 ps;
		fnEffectLoaded fn;
	};

	typedef string2 Filename;
	typedef std::map< std::pair<Filename, int>, std::vector<ShaderCallbackData> > ShaderCallbacks;
	typedef std::map< Filename, std::vector<fnStateLoaded> > StateCallbacks;
	typedef std::map< Filename, std::vector<fnSceneLoaded> > SceneCallbacks;
	typedef std::map< Filename, std::vector<fnMaterialsLoaded> > MaterialCallbacks;

	ShaderCallbacks _shader_callbacks;
	StateCallbacks _state_callbacks;
	SceneCallbacks _scene_callbacks;
	MaterialCallbacks _material_callbacks;
};

#endif
