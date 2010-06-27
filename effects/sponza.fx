Texture2D diffuse_texture;
Texture2D alpha_texture;
extern sampler g_sampler;

matrix mtx;

bool use_specular_map = false;
bool use_alpha_map = false;
bool use_bump_map = false;

struct vsInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
};

struct vsOutput
{
	float3 normal : NORMAL;
	float2 tex : TEXCOORD;
	float4 position : SV_POSITION;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(v.position, mtx);
	o.normal = v.normal;
	o.tex = v.tex;
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	if (use_alpha_map) {
		float4 a = alpha_texture.Sample(g_sampler, v.tex);
		if (a.a == 0)
			clip(-1);
		return a;
	}
	
	float3 light_dir = normalize(float3(0,-10,0));
	return (0.5 + 0.5 * dot(-light_dir, v.normal)) * diffuse_texture.Sample(g_sampler, v.tex);
}
