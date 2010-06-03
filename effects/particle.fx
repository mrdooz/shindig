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
	o.position = v.position;
	return o;
}

  // triangle strip
  // 1 3
  // 0 2
  
float3 verts[] = { 
	{-1, -1, 0},
	{-1, +1, 0},
	{+1, -1, 0},
	{+1, +1, 0} };
	
float scale = 0.1;	

[maxvertexcount(4)]
void gsMain(point vsOutput input[1], inout TriangleStream<gsOutput> output)
{
	gsOutput g;
	for (int i = 0; i < 4; ++i) {
		g.position = mul(float4(input[0].position + scale * verts[i], 1), mtx);
		output.Append(g);
	}
	output.RestartStrip();
}

float4 psMain(in gsOutput v) : SV_TARGET
{
	return float4(1,1,0,1);
}
