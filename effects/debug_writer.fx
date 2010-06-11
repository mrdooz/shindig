// Shader that just outputs a full screen quad

Texture2D g_texture;
extern sampler g_sampler;

struct psInput
{
	float4	pos : SV_Position;
	float2 tex : TexCoord;
};

// 0-1
// 2-3

psInput vsMain(in uint v : SV_VertexID)
{
	psInput o = (psInput)0;
	if (v == 0) {
		o.pos = float4(-1, +1, 0, 1);
		o.tex = float2(0,0);
	} else if (v == 1) {
		o.pos = float4(+1, +1, 0, 1);
		o.tex = float2(1,0);
	} else if (v == 2) {
		o.pos = float4(-1, -1, 0, 1);
		o.tex = float2(0,1);
	} else if (v == 3) {
		o.pos = float4(+1, -1, 0, 1);
		o.tex = float2(1,1);
	}
	return o;
}

float4 psMain(in psInput v) : SV_Target
{
	return g_texture.Sample(g_sampler, v.tex);
}
