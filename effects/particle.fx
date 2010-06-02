matrix mtx;
float4 color;

struct vsInput
{
	float3 position : POSITION;
};

struct vsOutput
{
	float3 position : POSITION;
};

struct gsOutput
{
	float4 position : SV_POSITION;
};


vsOutput vsMain( in vsInput v )
{
	vsOutput o;
	o.position = mul(float4(v.position,1), mtx);
	return o;
}

[maxvertexcount(4)]
void gsMain(point vsOutput input[1], inout TriangleStream<gsOutput> output)
{
	gsOutput g;
	g.position = float4(input[0].position, 1);
	output.Append(g);
	output.RestartStrip();
}

float4 psMain(in gsOutput v) : SV_TARGET
{
	return float4(1,1,0,1);
}
