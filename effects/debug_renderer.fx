matrix mtx;

struct vsInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct vsOutput
{
	float4 color : COLOR;
	float4 position : SV_POSITION;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(v.position, mtx);
	o.color = v.color;
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	return v.color;
}
