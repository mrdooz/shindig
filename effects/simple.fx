// shader that just uses pos

matrix mtx;

struct vsInput
{
	float4 position : POSITION;
};

struct vsOutput
{
	float4 position : SV_POSITION;
};

vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(v.position, mtx);
	return o;
}

float4 psMain(in vsOutput v) : SV_TARGET
{
	return float4(1,1,1,1);
}
