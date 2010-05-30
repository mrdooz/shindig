matrix mtx;
float4 color;

struct vsInput
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct vsOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(float4(v.position,1), mtx);
	o.color = v.color;
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	return v.color;
}
