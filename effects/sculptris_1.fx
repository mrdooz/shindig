matrix mtx;

struct vsInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct vsOutput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o = (vsOutput)0;
	o.position = mul(float4(v.position,1), mtx);
	o.normal = v.normal;
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	float3 light_dir = float3(0,-1,0);
	return saturate(dot(-light_dir, v.normal));
}
