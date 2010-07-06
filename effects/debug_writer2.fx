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
	//return float4(1,1,1,1);
	float4 col = g_texture.Sample(g_sampler, v.tex);
	clip(col.a == 0 ? -1 : 1);
	return g_texture.Sample(g_sampler, v.tex);
}
