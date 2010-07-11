#pragma once

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
};

struct lua_State;
bool lua_init(lua_State **ll, const char *filename);
float get_float_field(lua_State *l, const char *key);
int get_int_field(lua_State *l, const char *key);

bool blend_state_from_lua(const char *filename, const char *state_name, CComPtr<ID3D11BlendState>& blend_state);
bool depth_stencil_state_from_lua(const char *filename, const char *state_name, CComPtr<ID3D11DepthStencilState>& dss);
bool sampler_from_lua(const char *filename, const char *sampler_name, CComPtr<ID3D11SamplerState>& sampler);

bool load_states(const char *filename, const char *blend, const char *depth, const char *sampler, ID3D11BlendState **b, ID3D11DepthStencilState **d, ID3D11SamplerState **s);
