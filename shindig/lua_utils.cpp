#include "stdafx.h"
#include "lua_utils.hpp"
extern "C"
{
  #include <lstate.h>
  #include <ldo.h>
}

bool blend_state_from_lua(lua_State *l, const char *state_name, CComPtr<ID3D11BlendState>& blend_state);
bool depth_stencil_state_from_lua(lua_State *l, const char *state_name, CComPtr<ID3D11DepthStencilState>& dss);
bool sampler_from_lua(lua_State *l, const char *sampler_name, CComPtr<ID3D11SamplerState>& sampler);
bool rasterizer_state_from_lua(lua_State *l, const char *state_name, CComPtr<ID3D11RasterizerState>& state);


int get_int_field(lua_State *l, const char *key)
{
	SCOPED_OBJ([l](){lua_pop(l, 1); });	// pop the value off the stack
	// assumes the table with the field is pushed on the stack
	lua_pushstring(l, key);
	lua_gettable(l, -2);	// pop key, and push value on stack. the table is at -2
	if (!lua_isnumber(l, -1)) {
		LOG_WARNING_LN("Error reading value for key: %s", key);
		throw std::runtime_error(__FUNCTION__);
	}
	return (int)((int64_t)lua_tonumber(l, -1));
}

float get_float_field(lua_State *l, const char *key)
{
	SCOPED_OBJ([l](){lua_pop(l, 1); });	// pop the value off the stack
	// assumes the table with the field is pushed on the stack
	lua_pushstring(l, key);
	lua_gettable(l, -2);	// pop key, and push value on stack
	if (!lua_isnumber(l, -1)) {
		LOG_WARNING_LN("Error reading value for key: %s", key);
		throw std::runtime_error(__FUNCTION__);
	}
	return (float)lua_tonumber(l, -1);
}

bool lua_init(lua_State **ll, const char *filename)
{
	lua_State *l = *ll = lua_open();
	if (l == NULL)
		return false;
	luaL_openlibs(l);

	if (luaL_loadfile(l,  filename)) {
		LOG_WARNING_LN(lua_tostring(l, -1));
		return false;
	}

	// update the package path to include the script's directory
	luaL_dostring(l, string2::fmt("package.path = '%s/?.lua;' .. package.path", Path::get_path(filename)));

	return true;
}

bool blend_state_from_lua(lua_State *l, const char *state_name, CComPtr<ID3D11BlendState>& blend_state)
{
	// push state table on stack
	lua_getglobal(l, state_name);

#define CREATE_DESC(x) \
	lua_pushinteger(l, x);  \
	lua_gettable(l, -2); \
	D3D11_RENDER_TARGET_BLEND_DESC b ## x = { \
	get_int_field(l, "BlendEnable"), \
	(D3D11_BLEND)get_int_field(l, "SrcBlend"), \
	(D3D11_BLEND)get_int_field(l, "DestBlend"), \
	(D3D11_BLEND_OP)get_int_field(l, "BlendOp"), \
	(D3D11_BLEND)get_int_field(l, "SrcBlendAlpha"), \
	(D3D11_BLEND)get_int_field(l, "DestBlendAlpha"), \
	(D3D11_BLEND_OP)get_int_field(l, "BlendOpAlpha"), \
	get_int_field(l, "RenderTargetWriteMask") }; \
	lua_pop(l, 1);

	CREATE_DESC(0);
	CREATE_DESC(1);
	CREATE_DESC(2);
	CREATE_DESC(3);
	CREATE_DESC(4);
	CREATE_DESC(5);
	CREATE_DESC(6);
	CREATE_DESC(7);

  blend_state.Attach(rt::D3D11::BlendDescription().
    AlphaToCoverageEnable_(get_int_field(l, "AlphaToCoverageEnable")).
    IndependentBlendEnable_(get_int_field(l, "IndependentBlendEnable")).
    RenderTarget_(0, b0).
    RenderTarget_(1, b1).
    RenderTarget_(2, b2).
    RenderTarget_(3, b3).
    RenderTarget_(4, b4).
    RenderTarget_(5, b5).
    RenderTarget_(6, b6).
    RenderTarget_(7, b7).
    Create(Graphics::instance().device()));

  return !!blend_state.p;
}

bool depth_stencil_state_from_lua(lua_State *l, const char *state_name, CComPtr<ID3D11DepthStencilState>& dss)
{
	// push sampler table on stack
	lua_getglobal(l, state_name);

  dss.Attach(rt::D3D11::DepthStencilDescription().
    DepthEnable_(get_int_field(l, "DepthEnable")).
    DepthWriteMask_((D3D11_DEPTH_WRITE_MASK)get_int_field(l, "DepthWriteMask")).
    DepthFunc_((D3D11_COMPARISON_FUNC)get_int_field(l, "DepthFunc")).
    StencilEnable_(get_int_field(l, "StencilEnable")).
    StencilReadMask_(get_int_field(l, "StencilReadMask")).
    StencilWriteMask_(get_int_field(l, "StencilWriteMask")).
    FrontStencilFailOp_((D3D11_STENCIL_OP)get_int_field(l, "FrontStencilFailOp")).
    FrontStencilDepthFailOp_((D3D11_STENCIL_OP)get_int_field(l, "FrontStencilDepthFailOp")).
    FrontStencilPassOp_((D3D11_STENCIL_OP)get_int_field(l, "FrontStencilPassOp")).
    FrontStencilFunc_((D3D11_COMPARISON_FUNC)get_int_field(l, "FrontStencilFunc")).
    BackStencilFailOp_((D3D11_STENCIL_OP)get_int_field(l, "BackStencilFailOp")).
    BackStencilDepthFailOp_((D3D11_STENCIL_OP)get_int_field(l, "BackStencilDepthFailOp")).
    BackStencilPassOp_((D3D11_STENCIL_OP)get_int_field(l, "BackStencilPassOp")).
    BackStencilFunc_((D3D11_COMPARISON_FUNC)get_int_field(l, "BackStencilFunc")).
    Create(Graphics::instance().device()));

  return !!dss.p;
}

// helper function to set a D3D11_SAMPLER_DESC
bool sampler_from_lua(lua_State *l, const char *sampler_name, CComPtr<ID3D11SamplerState>& sampler)
{
	// TODO: add support for border

	// push sampler table on stack
	lua_getglobal(l, sampler_name);
  sampler.Attach(rt::D3D11::SamplerDescription().
    AddressU_((D3D11_TEXTURE_ADDRESS_MODE)get_int_field(l, "AddressU")).
    AddressV_((D3D11_TEXTURE_ADDRESS_MODE)get_int_field(l, "AddressV")).
    Filter_((D3D11_FILTER)get_int_field(l, "Filter")).
    MipLODBias_(get_float_field(l, "MipLODBias")).
    MaxAnisotropy_(get_int_field(l, "MaxAnisotropy")).
    ComparisonFunc_((D3D11_COMPARISON_FUNC)get_int_field(l, "ComparisonFunc")).
    //BorderColor_(0, )
    MinLOD_(get_float_field(l, "MinLOD")).
    MaxLOD_(get_float_field(l, "MaxLOD")).
    Create(Graphics::instance().device()));

  return !!sampler.p;
}

bool rasterizer_state_from_lua(lua_State *l, const char *state_name, CComPtr<ID3D11RasterizerState>& state)
{
  lua_getglobal(l, state_name);
  state.Attach(rt::D3D11::RasterizerDescription().
    FillMode_((D3D11_FILL_MODE)get_int_field(l, "FillMode")).
    CullMode_((D3D11_CULL_MODE)get_int_field(l, "CullMode")).
    FrontCounterClockwise_(get_int_field(l, "FrontCounterClockwise")).
    DepthBias_(get_int_field(l, "DepthBias")).
    DepthBiasClamp_(get_float_field(l, "DepthBiasClamp")).
    SlopeScaledDepthBias_(get_float_field(l, "SlopeScaledDepthBias")).
    DepthClipEnable_(get_int_field(l, "DepthClipEnable")).
    ScissorEnable_(get_int_field(l, "ScissorEnable")).
    MultisampleEnable_(get_int_field(l, "MultisampleEnable")).
    AntialiasedLineEnable_(get_int_field(l, "AntialiasedLineEnable")).
    Create(Graphics::instance().device()));

  return !!state.p;
}

bool lua_load_states(const char *filename, 
  const char *blend, const char *depth, const char *sampler, const char *rasterizer, 
  ID3D11BlendState **b, ID3D11DepthStencilState **d, ID3D11SamplerState **s, ID3D11RasterizerState **r)
{
  lua_State *l = NULL;
  if (!lua_init(&l, filename))
    return false;

  SCOPED_OBJ([l](){lua_close(l); } );

  if (lua_pcall(l, 0, 0, 0)) {
    LOG_WARNING_LN(lua_tostring(l, -1));
    return false;
  }

  // XXX: To get this to work, the lua_longjmp struct must be moved from ldo.c to ldo.h. There
  // has to be a cleaner way to do this..
/*
  lua_longjmp jmp;
  jmp.previous = l->errorJmp;
  jmp.status = 0;
  l->errorJmp = &jmp;

  int err;
  if (!(err = setjmp(jmp.b))) {
*/
    CComPtr<ID3D11BlendState> tmp_blend;
    CComPtr<ID3D11SamplerState> tmp_sampler;
    CComPtr<ID3D11DepthStencilState> tmp_dss;
    CComPtr<ID3D11RasterizerState> tmp_rast;

    if (blend && !blend_state_from_lua(l, blend, tmp_blend))
      return false;

    if (depth && !depth_stencil_state_from_lua(l, depth, tmp_dss))
      return false;

    if (sampler && !sampler_from_lua(l, sampler, tmp_sampler))
      return false;

    if (rasterizer && !rasterizer_state_from_lua(l, rasterizer, tmp_rast))
      return false;

    if (b) *b = tmp_blend.Detach();
    if (d) *d = tmp_dss.Detach();
    if (s) *s = tmp_sampler.Detach();
    if (r) *r = tmp_rast.Detach();
/*
  } else {
    LOG_ERROR_LN("Error loading lua state: %s", filename);
    return false;
  }
*/
	return true;
}
