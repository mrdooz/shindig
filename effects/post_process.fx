Texture2D texture;

SamplerState linear_sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct vsInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

struct vsOutput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o = (vsOutput)0;
	o.position = v.position;	
	o.tex = v.tex;
	return o;
}

float4 Transform(in vsInput v) : SV_POSITION 
{
    return v.position;
}

float4 psMain2(float4 v : SV_POSITION) : SV_TARGET
{
	return float4(1,0,1,0);
}

float4 psMain(vsOutput v) : SV_TARGET
{
	return texture.sample(linear_sampler, v.tex);
}
