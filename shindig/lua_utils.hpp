#pragma once

bool blend_state_from_lua(const char *filename, const char *state_name, CComPtr<ID3D11BlendState>& blend_state);
bool depth_stencil_state_from_lua(const char *filename, const char *state_name, CComPtr<ID3D11DepthStencilState>& dss);
bool sampler_from_lua(const char *filename, const char *sampler_name, CComPtr<ID3D11SamplerState>& sampler);

bool load_states(const char *filename, const char *blend, const char *depth, const char *sampler, ID3D11BlendState **b, ID3D11DepthStencilState **d, ID3D11SamplerState **s);
