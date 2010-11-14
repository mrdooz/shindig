require "dx"

local my_sampler = dx.sampler_state
my_sampler["AddressU"] = dx.D3D11_TEXTURE_ADDRESS_WRAP
my_sampler["AddressV"] = dx.D3D11_TEXTURE_ADDRESS_WRAP
my_sampler["AddressW"] = dx.D3D11_TEXTURE_ADDRESS_WRAP
my_sampler["Filter"] = dx.D3D11_FILTER_MIN_MAG_MIP_LINEAR


default_sampler = my_sampler
default_blend = dx.blend_state
default_dss = dx.depth_stencil_state
