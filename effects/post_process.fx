Texture2D g_MeshTexture;              // Color texture for mesh
extern sampler MeshTextureSampler;

struct vsInput
{
	float4 position : SV_Position;
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

float4 Transform(in vsInput v) : SV_Position 
{
    return v.position;
}

float4 psMain2(float4 v : SV_Position) : SV_Target
{
	return float4(1,0,1,0);
}

float4 psMain(vsOutput v) : SV_Target
{
	//return float4(0,1,1,0);
	float4 tmp = g_MeshTexture.Sample(MeshTextureSampler, v.tex);
	return g_MeshTexture.Sample(MeshTextureSampler, float2(v.tex.x + tmp.r / 100, v.tex.y + tmp.g / 100));
}
