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
	for (std::vector<ShaderCallbackData>::const_iterator i = n.begin(), e = n.end(); i != e; ++i) {
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

bool ResourceManager::reload_effect_states(const std::string& filename)
{
	// load the effect state file
	uint32_t len = 0;
	uint8_t* buf = load_file(filename.c_str(), true, &len);
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

	BlendStates blend_states;
	for (BlendDescs::iterator i = b._blend_descs.begin(), e = b._blend_descs.end(); i != e; ++i) {
		ID3D11BlendState* s;
		Graphics::instance().device()->CreateBlendState(&i->second, &s);
		blend_states.insert(std::make_pair(i->first, s));
	}

	// call the callbacks that are watching this file
	std::vector<fnStateLoaded>& callbacks = _state_callbacks[filename];
	for (std::vector<fnStateLoaded>::iterator i = callbacks.begin(), e = callbacks.end(); i != e; ++i) {
		(*i)(blend_states);
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

bool ResourceManager::reload_material(const char* filename)
{
	std::ifstream is(filename);
	json_spirit::mValue value;
	if (!json_spirit::read( is, value )) {
		return false;
	}
	//const json_spirit::mArray& addr_array = value.get_array();
	const json_spirit::mObject& addr_array = value.get_obj();

	for (json_spirit::mObject::const_iterator i = addr_array.begin(), e = addr_array.end(); i != e; ++i) {
		const json::mValue v = i->second;
		int a = 10;
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

	for (std::vector<fnSceneLoaded>::iterator i = it->second.begin(), e = it->second.end(); i != e; ++i) {
		(*i)(scene);
	}

	scene->release();

	return true;
}
