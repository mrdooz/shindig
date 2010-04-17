sampler MeshTextureSampler = 
sampler_state
{
    Texture = <g_MeshTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
    SrcBlend = One;
    DestBlend = Zero;
    BlendOp = Add;
    SrcBlendAlpha = One;
    DestBlendAlpha = Zero;
    BlendOpAlpha = Add;
    RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState rastah_man
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = False;
	DepthBias = 0;
	DepthBiasClamp = 0.1;
	SlopeScaledDepthBias = 0.2;
	DepthClipEnable = True;
	ScissorEnable = False;
	MultisampleEnable = False;
	AntialiasedLineEnable = False;
#	desc->FillMode = D3D11_FILL_SOLID;
#	desc->CullMode = D3D11_CULL_BACK;
#	desc->FrontCounterClockwise = FALSE;
#	desc->DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
#	desc->DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
#	desc->SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
#	desc->DepthClipEnable = TRUE;
#	desc->ScissorEnable = FALSE;
#	desc->MultisampleEnable = FALSE;
#	desc->AntialiasedLineEnable = FALSE;
};