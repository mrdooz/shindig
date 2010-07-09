require "dx"

local my_sampler = dx.default_sampler()
my_sampler["AddressU"] = dx.D3D11_TEXTURE_ADDRESS_WRAP
my_sampler["AddressV"] = dx.D3D11_TEXTURE_ADDRESS_WRAP
my_sampler["AddressW"] = dx.D3D11_TEXTURE_ADDRESS_WRAP
my_sampler["Filter"] = dx.D3D11_FILTER_MIN_MAG_MIP_LINEAR

local my_blend = dx.default_blend()

default_sampler = my_sampler
default_blend = my_blend
default_dss = dx.default_dss()
