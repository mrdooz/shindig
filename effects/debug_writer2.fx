// coordinates are in clip space

Texture2D g_texture;
extern sampler g_sampler;

struct vsInput
{
	float4 pos : SV_Position;
	float2 tex : TexCoord;
};

struct psInput
{
	float4	pos : SV_Position;
	float2 tex : TexCoord;
};

psInput vsMain(in vsInput v)
{
	psInput o;
	o.pos = v.pos;
	o.tex = v.tex;
	return o;
}

float4 psMain(in psInput v) : SV_Target
{
	return g_texture.Sample(g_sampler, v.tex);
}
