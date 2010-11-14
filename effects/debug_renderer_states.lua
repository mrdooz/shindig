require "dx"

dss = dx.depth_stencil_state
bs = dx.blend_state
rs = dx.rasterizer_state

dss["DepthEnable"] = 1
dss["DepthWriteMask"] = dx.D3D11_DEPTH_WRITE_MASK_ZERO

rs["CullMode"] = dx.D3D11_CULL_NONE
