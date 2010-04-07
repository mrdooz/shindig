#include "stdafx.h"
#include "resource_manager.hpp"
#include "parser/state_parser.hpp"
#include "graphics.hpp"
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
	_shader_callbacks[filename].push_back(std::make_pair(shader_name, fn));
	return reload_vertex_shader(filename);
}

bool ResourceManager::reload_vertex_shader(const char* filename)
{
	// find all the callbacks that use this file
	if (_shader_callbacks.find(filename) == _shader_callbacks.end()) {
		return true;
	}

	NameAndCallbacks& n = _shader_callbacks[filename];
	for (NameAndCallbacks::iterator i = n.begin(), e = n.end(); i != e; ++i) {
		const std::string& shader_name = i->first;
		fnEffectLoaded fn = i->second;

		EffectWrapper ew;
		ew.load(filename, shader_name.c_str());
		fn(ew);
	}
	return true;
}

bool ResourceManager::load_pixel_shader(const char* filename, const char* shader_name, const fnEffectLoaded& fn)
{
	return true;
}

bool ResourceManager::reload_pixel_shader(const char* filename)
{
	return true;
}

bool ResourceManager::load_effect_states(const char* filename, const fnStateLoaded& fn)
{
	_state_callbacks[filename].e.push_back(fn);
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

	for (BlendDescs::iterator i = b._blend_descs.begin(), e = b._blend_descs.end(); i != e; ++i) {
		ID3D11BlendState* s;
		Graphics::instance().device()->CreateBlendState(&i->second, &s);
		_blend_states.insert(std::make_pair(i->first, s));
	}

	// call the callbacks that are watching this file
	Callbacks& callbacks = _state_callbacks[filename];
	for (Callbacks::E::iterator i = callbacks.e.begin(), e = callbacks.e.end(); i != e; ++i) {
		(*i)(_blend_states);
	}

	return true;

}
