Texture2D g_MeshTexture;              // Color texture for mesh
extern sampler MeshTextureSampler;

struct vsInput
{
	float3 position : POSITION;
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
	o.position = float4(v.position, 1.0f);	
	o.tex = v.tex;
	return o;
}

float4 psMain(vsOutput input) : SV_Target
{
	return float4(1,0,1,0);
	return g_MeshTexture.Sample(MeshTextureSampler, input.tex);
}
