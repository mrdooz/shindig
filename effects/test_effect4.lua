require "dx"

local my_sampler = dx.sampler_state
my_sampler["AddressU"] = dx.D3D11_TEXTURE_ADDRESS_CLAMP
my_sampler["AddressV"] = dx.D3D11_TEXTURE_ADDRESS_CLAMP
my_sampler["Filter"] = dx.D3D11_FILTER_MIN_MAG_MIP_LINEAR

local my_blend = dx.blend_state
my_blend[0]["BlendEnable"] = 0

local my_dss = dx.depth_stencil_state
my_dss["DepthEnable"] = 0

default_sampler = my_sampler
default_blend = my_blend
default_dss = my_dss
