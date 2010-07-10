// draw a full screen textured quad
Texture2D g_texture;
extern sampler g_sampler;


// 1 3
// 0 2

static float3 g_positions[4] =
{
	float3( -1, 1, 0 ),
	float3( 1, 1, 0 ),
	float3( -1, -1, 0 ),
	float3( 1, -1, 0 ),
};

static float2 g_texcoords[4] =
{
	float2(0, 1),
	float2(0, 0),
	float2(1, 1),
	float2(1, 0),
};

struct psInput
{
	float4	pos : SV_Position;
	float2 tex : TexCoord;
};

struct vsInput
{
	float4 pos : SV_Position;
	float2 tex : TexCoord;
};

psInput vsMain(in vsInput v)
{
	psInput o;
	o.pos = v.pos;
	o.tex = v.tex;
	return o;
}
/*
psInput vsMain(in uint v : SV_VertexID)
{
	psInput o = (psInput)0;
	o.pos = float4(g_positions[v], 1);
	o.tex = g_texcoords[v];
	return o;
}
*/
float4 psMain(in psInput v) : SV_TARGET
{
	return g_texture.Sample(g_sampler, v.tex);
}
