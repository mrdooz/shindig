Texture2D g_texture;
extern sampler g_sampler;

struct vsInput
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct psInput
{
	float4	pos : SV_Position;
	float2 tex : TexCoord;
};

psInput vsMain(in vsInput v)
{
	psInput o;
	o.pos = float4(v.pos, 1);
	o.tex = v.tex;
	return o;
}

float4 psMain(in psInput v) : SV_Target
{
	return g_texture.Sample(g_sampler, v.tex);
}
