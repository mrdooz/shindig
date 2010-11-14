#pragma once

struct lua_State;
bool lua_init(lua_State **ll, const char *filename);
float get_float_field(lua_State *l, const char *key);
int get_int_field(lua_State *l, const char *key);

bool lua_load_states(const char *filename, 
  const char *blend, const char *depth, const char *sampler, const char *rasterizer,
  ID3D11BlendState **b, ID3D11DepthStencilState **d, ID3D11SamplerState **s, ID3D11RasterizerState **r);
