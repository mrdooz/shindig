matrix mtx;
float4 color;
Texture2D g_texture;
extern sampler g_sampler;

struct vsInput
{
	float3 position : POSITION;
	float scale : TEXCOORD;
};

struct vsOutput
{
	float3 position : POSITION;
	float scale : TEXCOORD;
};

struct gsOutput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};


vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = v.position;
	o.scale = v.scale;
	return o;
}

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

static float g_scale = 0.03;

[maxvertexcount(4)]
void gsMain(point vsOutput input[1], inout TriangleStream<gsOutput> output)
{
	gsOutput g = (gsOutput)0;
	for (int i = 0; i < 4; i++) {
		g.position = mul(float4(input[0].position + g_scale * g_positions[i], 1), mtx);
		g.texcoord = g_texcoords[i];
		output.Append(g);
	}
	output.RestartStrip();
}

float4 psMain(in gsOutput v) : SV_TARGET
{
	return g_texture.Sample(g_sampler, v.texcoord);
}
