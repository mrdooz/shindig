matrix mtx;
float4 color;

struct vsInput
{
	float3 position : POSITION;
};

struct vsOutput
{
	float4 position : SV_POSITION;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o = (vsOutput)0;
	o.position = mul(float4(v.position,1), mtx);
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	return color;
}
