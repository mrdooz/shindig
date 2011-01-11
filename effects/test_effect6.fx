matrix mtx;

float3 light = float3(0,100,100);

struct vsInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct vsOutput
{
	float4 position : SV_POSITION;
	float3 ws_pos : TEXCOORD0;
	float3 normal : NORMAL;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(float4(v.position,1), mtx);
	o.ws_pos = v.position;
	o.normal = v.normal; // mul(float4(v.normal,0), mtx);
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	return float4(v.normal,1);
	return 0.5f * dot(normalize(light - v.ws_pos), v.normal);
}
