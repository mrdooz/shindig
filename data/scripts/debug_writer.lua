require "dx"

local my_sampler = dx.default_sampler()
my_sampler["AddressU"] = dx.D3D11_TEXTURE_ADDRESS_CLAMP
my_sampler["AddressV"] = dx.D3D11_TEXTURE_ADDRESS_CLAMP
my_sampler["Filter"] = dx.D3D11_FILTER_MIN_MAG_MIP_LINEAR

local my_blend = dx.default_blend()
my_blend[0]["BlendEnable"] = 1
my_blend[0]["BlendOp"] = dx.D3D11_BLEND_OP_ADD
my_blend[0]["SrcBlend"] = dx.D3D11_BLEND_SRC_COLOR
my_blend[0]["DestBlend"] = dx.D3D11_BLEND_DEST_COLOR
my_blend[0]["BlendOpAlpha"] = dx.D3D11_BLEND_OP_ADD
my_blend[0]["SrcBlendAlpha"] = dx.D3D11_BLEND_SRC_ALPHA
my_blend[0]["DestBlendAlpha"] = dx.D3D11_BLEND_INV_SRC_ALPHA

local my_dss = dx.default_dss()
my_dss["DepthEnable"] = 0

default_sampler = my_sampler
default_blend = my_blend
default_dss = my_dss
