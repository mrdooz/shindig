#include "stdafx.h"
#include "resource_manager.hpp"
#include "redux_loader.hpp"
#include "scene.hpp"
#include "system.hpp"
#include <celsus/file_utils.hpp>

ResourceManager* ResourceManager::_instance = NULL;

using namespace fastdelegate;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

ResourceManager& ResourceManager::instance()
{
	if (_instance == NULL) {
		_instance = new ResourceManager();
	}

	return *_instance;
}

// This can be removed once I stop being stupid and fix a proper bind
struct Trampoline
{
  typedef std::function<bool(const string2&, int)> Fn;
  Trampoline(const Fn& fn, const int flags) : _fn(fn), _flags(flags) {}

  bool run(const string2& str)
  {
    bool res = _fn(str, _flags);
    return res;
  }
  Fn _fn;
  int _flags;
};

bool ResourceManager::load_shaders(const char *filename, const char *vs, const char *gs, const char *ps, const fnEffectLoaded& fn)
{
	auto f = Path::make_canonical(Path::get_full_path_name(filename));
	const int flags = !!vs * kVertexShader | !!gs * kGeometryShader | !!ps * kPixelShader;
	_shader_callbacks[std::make_pair(f, flags)].push_back(ShaderCallbackData(f, vs, gs, ps, fn));
  auto ff = [this, flags](const char *filename) { return reload_shader(filename, flags); };
	return System::instance().add_file_changed(f, ff, true);
//    MakeDelegate(&Trampoline(MakeDelegate(this, &ResourceManager::reload_shader), flags), &Trampoline::run), true);
}

bool ResourceManager::reload_shader(const char* filename, const int shaders)
{
	// find all the callbacks that use this file
  auto key = std::make_pair(filename, shaders);
  auto it = _shader_callbacks.find(key);
	if (_shader_callbacks.find(key) == _shader_callbacks.end()) {
		return true;
	}

	const std::vector<ShaderCallbackData>& n = it->second;
	for (auto i = n.begin(), e = n.end(); i != e; ++i) {
    EffectWrapper* effect = new EffectWrapper();
		if (!effect->load_shaders(i->filename, 
			shaders & kVertexShader ? i->vs : NULL, 
			shaders & kGeometryShader ? i->gs : NULL, 
			shaders & kPixelShader ? i->ps : NULL))
			return false;

		i->fn(effect);
	}
	return true;
}

bool ResourceManager::load_scene(const char* filename, const fnSceneLoaded& fn)
{
	SUPER_ASSERT(_scene_callbacks.find(filename) == _scene_callbacks.end());
	_scene_callbacks[filename].push_back(fn);
	return reload_scene(filename);
}

bool ResourceManager::load_materials(const char* filename, const fnMaterialsLoaded& fn)
{
	SUPER_ASSERT(_material_callbacks.find(filename) == _material_callbacks.end());
	_material_callbacks[filename].push_back(fn);
	return reload_material(filename);
}

namespace json = json_spirit;

template<class T, class U>
bool find_obj(const T& obj, const string2& name, U** out)
{
	for (auto i = obj.begin(), e = obj.end(); i != e; ++i) {
		if (i->first.c_str() == name) {
			*out = &(i->second);
			return true;
		}
	}
	return false;
}

D3DXCOLOR array_to_color(const json_spirit::mValue::Array& arr)
{
	return D3DXCOLOR((float)arr[0].get_real(), (float)arr[1].get_real(), (float)arr[2].get_real(), (float)arr[3].get_real());
}

bool ResourceManager::reload_material(const char* filename)
{
	MaterialCallbacks::iterator it = _material_callbacks.find(filename);
	if (it == _material_callbacks.end()) {
		LOG_WARNING_LN("unable to find callbacks for scene: %s", filename);
		return false;
	}

	std::ifstream is(filename);
	json_spirit::mValue global;
	if (!json_spirit::read(is, global)) {
		return false;
	}

	MaterialFile material_file;
	const json_spirit::mValue* materials = NULL;
	const json_spirit::mValue* material_connections = NULL;
	const json_spirit::mValue* effect_connections = NULL;
	RETURN_ON_FAIL_BOOL(find_obj(global.get_obj(), "materials", &materials), LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(find_obj(global.get_obj(), "material_connections", &material_connections), LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(find_obj(global.get_obj(), "effect_connections", &effect_connections), LOG_ERROR_LN);

	// parse the materials
	for (auto i = materials->get_array().begin(), e = materials->get_array().end(); i != e; ++i) {
		// parse a single material
		auto single_material = i->get_obj();
		const json_spirit::mValue* name = NULL;
		const json_spirit::mValue* values = NULL;
		RETURN_ON_FAIL_BOOL(find_obj(single_material, "name", &name), LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(find_obj(single_material, "values", &values), LOG_ERROR_LN);
		MaterialFile::Material m;
		m.name = name->get_str();
		for (auto i = values->get_array().begin(), e = values->get_array().end(); i != e; ++i) {
			auto single_value = *i;
			const json_spirit::mValue* name = NULL;
			const json_spirit::mValue* type = NULL;
			const json_spirit::mValue* value = NULL;
			RETURN_ON_FAIL_BOOL(find_obj(single_value.get_obj(), "name", &name), LOG_ERROR_LN);
			RETURN_ON_FAIL_BOOL(find_obj(single_value.get_obj(), "type", &type), LOG_ERROR_LN);
			RETURN_ON_FAIL_BOOL(find_obj(single_value.get_obj(), "value", &value), LOG_ERROR_LN);
			if (*name == "transparency") {
				m.transparency = (float)value->get_real();
			} else if (*name == "ambient_color") {
				m.ambient = array_to_color(value->get_array());
			} else if (*name == "diffuse_color") {
				m.diffuse = array_to_color(value->get_array());
			} else if (*name == "emissive_color") {
				m.emissive = array_to_color(value->get_array());
			}
		}
		material_file.materials.insert(std::make_pair(m.name, m));
	}

	// parse the material connections
	for (auto i = material_connections->get_array().begin(), e = material_connections->get_array().end(); i != e; ++i) {
		auto single_connection = i->get_obj();
		const json_spirit::mValue* mesh = NULL;
		const json_spirit::mValue* material = NULL;
		RETURN_ON_FAIL_BOOL(find_obj(single_connection, "mesh", &mesh), LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(find_obj(single_connection, "material", &material), LOG_ERROR_LN);
		material_file.material_connections.insert(std::make_pair(mesh->get_str(), MaterialFile::MaterialConnection(mesh->get_str(), material->get_str())));
	}

	for (auto i = it->second.begin(), e = it->second.end(); i != e; ++i) {
		(*i)(material_file);
	}

	return true;
}

bool ResourceManager::reload_scene(const char* filename)
{
	Scene* scene = new Scene();
	ReduxLoader loader(filename, scene, NULL);
	RETURN_ON_FAIL_BOOL(loader.load(), LOG_ERROR_LN);

	SceneCallbacks::iterator it = _scene_callbacks.find(filename);
	if (it == _scene_callbacks.end()) {
		LOG_WARNING_LN("unable to find callbacks for scene: %s", filename);
		return false;
	}

	for (auto i = it->second.begin(), e = it->second.end(); i != e; ++i) {
		(*i)(scene);
	}

	scene->release();

	return true;
}
