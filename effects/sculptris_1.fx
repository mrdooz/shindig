matrix mtx;

struct vsInput
{
	float4 position : POSITION;
	float3 normal : NORMAL;
};

struct vsOutput
{
	float3 normal : NORMAL;
	float4 position : SV_POSITION;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(v.position, mtx);
	o.normal = v.normal;
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	float3 light_dir = normalize(float3(0,-10,0));
	return 0.5f + 0.1f * dot(-light_dir, v.normal);
}
