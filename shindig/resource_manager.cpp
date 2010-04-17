#include "stdafx.h"
#include "resource_manager.hpp"
#include "parser/state_parser.hpp"
#include "graphics.hpp"
#include "redux_loader.hpp"
#include "scene.hpp"
#include "effect_wrapper.hpp"

ResourceManager* ResourceManager::_instance = NULL;

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

extern int parse_tokens(Tokens& tokens, char* str);
char* make_string(const char* ts, const char* te)
{
	const int len = te - ts;
	char* buf = new char[len+1];
	memcpy(buf, ts, len);
	buf[len] = 0;
	return buf;
}

int make_hex(const char* ts, const char* te)
{
	char* tmp = _strdup(ts);
	char* ptr = tmp;
	char* pe = ptr + (te - ts);
	while (ptr < pe) {
		*ptr++ = tolower(*ptr);
	}
	int value = 0;
	sscanf(tmp, "0x%x", &value);
	free(tmp);
	return value;
}

bool ResourceManager::load_vertex_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn)
{
	char buf[MAX_PATH];
	sprintf(buf, "[VS] - %s::%s", filename, shader_name);
	SUPER_ASSERT(_shader_callbacks.find(buf) == _shader_callbacks.end());
	_shader_callbacks[buf].push_back(ShaderCallbackData(filename, shader_name, fn));
	return reload_shader(buf, true);
}

bool ResourceManager::load_pixel_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn)
{
	char buf[MAX_PATH];
	sprintf(buf, "[PS] - %s::%s", filename, shader_name);
	SUPER_ASSERT(_shader_callbacks.find(buf) == _shader_callbacks.end());
	_shader_callbacks[buf].push_back(ShaderCallbackData(filename, shader_name, fn));
	return reload_shader(buf, false);
}

bool ResourceManager::reload_shader(const char* filename, const bool vertex_shader)
{
	// find all the callbacks that use this file
	if (_shader_callbacks.find(filename) == _shader_callbacks.end()) {
		return true;
	}

	const std::vector<ShaderCallbackData>& n = _shader_callbacks[filename];
	for (auto i = n.begin(), e = n.end(); i != e; ++i) {
		const std::string& filename = i->_filename;
		const std::string& entry_point = i->_entry_point;
		fnEffectLoaded fn = i->_effect_loaded;

    EffectWrapper* effect = new EffectWrapper();
		if (vertex_shader) {
			effect->load_vertex_shader(filename.c_str(), entry_point.c_str());
		} else {
			effect->load_pixel_shader(filename.c_str(), entry_point.c_str());
		}
		fn(effect);
	}
	return true;
}

bool ResourceManager::load_effect_states(const char* filename, const fnStateLoaded& fn)
{
	SUPER_ASSERT(_state_callbacks.find(filename) == _state_callbacks.end());
	_state_callbacks[filename].push_back(fn);
	return reload_effect_states(filename);
}

bool ResourceManager::reload_effect_states(const char* filename)
{
	// load the effect state file
	uint32_t len = 0;
	uint8_t* buf = load_file(filename, true, &len);
	if (buf == NULL) {
		return false;
	}

	// parse it
	Tokens tokens;
	parse_tokens(tokens, (char*)buf);
	StateParser p(tokens);
	if (!p.run()) {
		return false;
	}
	BigState b = p._states;

	ID3D11Device* device = Graphics::instance().device();

	EffectStates effect_states;

	// Collect blend states
	for (auto i = b._blend_descs.begin(), e = b._blend_descs.end(); i != e; ++i) {
		CComPtr<ID3D11BlendState> s;
		device->CreateBlendState(&i->second, &s);
		effect_states.blend_states.insert(std::make_pair(i->first, s));
	}

	// Collect sampler states
	SamplerStates sampler_states;
	for (auto i = b._sampler_descs.begin(), e = b._sampler_descs.end(); i != e; ++i) {
		CComPtr<ID3D11SamplerState> s;
		device->CreateSamplerState(&i->second, &s);
		effect_states.sampler_states.insert(std::make_pair(i->first, s));
	}


	// call the callbacks that are watching this file
	auto callbacks = _state_callbacks[filename];
	for (auto i = callbacks.begin(), e = callbacks.end(); i != e; ++i) {
		(*i)(effect_states);
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
bool find_obj(const T& obj, const std::string& name, U** out)
{
	for (auto i = obj.begin(), e = obj.end(); i != e; ++i) {
		if (i->first == name) {
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
	RETURN_ON_FAIL_BOOL(find_obj(global.get_obj(), "materials", &materials), ErrorPredicate<bool>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(find_obj(global.get_obj(), "material_connections", &material_connections), ErrorPredicate<bool>, LOG_ERROR_LN);
	RETURN_ON_FAIL_BOOL(find_obj(global.get_obj(), "effect_connections", &effect_connections), ErrorPredicate<bool>, LOG_ERROR_LN);

	// parse the materials
	for (auto i = materials->get_array().begin(), e = materials->get_array().end(); i != e; ++i) {
		// parse a single material
		auto single_material = i->get_obj();
		const json_spirit::mValue* name = NULL;
		const json_spirit::mValue* values = NULL;
		RETURN_ON_FAIL_BOOL(find_obj(single_material, "name", &name), ErrorPredicate<bool>, LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(find_obj(single_material, "values", &values), ErrorPredicate<bool>, LOG_ERROR_LN);
		MaterialFile::Material m;
		m.name = name->get_str();
		for (auto i = values->get_array().begin(), e = values->get_array().end(); i != e; ++i) {
			auto single_value = *i;
			const json_spirit::mValue* name = NULL;
			const json_spirit::mValue* type = NULL;
			const json_spirit::mValue* value = NULL;
			RETURN_ON_FAIL_BOOL(find_obj(single_value.get_obj(), "name", &name), ErrorPredicate<bool>, LOG_ERROR_LN);
			RETURN_ON_FAIL_BOOL(find_obj(single_value.get_obj(), "type", &type), ErrorPredicate<bool>, LOG_ERROR_LN);
			RETURN_ON_FAIL_BOOL(find_obj(single_value.get_obj(), "value", &value), ErrorPredicate<bool>, LOG_ERROR_LN);
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
		RETURN_ON_FAIL_BOOL(find_obj(single_connection, "mesh", &mesh), ErrorPredicate<bool>, LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(find_obj(single_connection, "material", &material), ErrorPredicate<bool>, LOG_ERROR_LN);
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
	RETURN_ON_FAIL_BOOL(loader.load(), ErrorPredicate<bool>, LOG_ERROR_LN);

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
