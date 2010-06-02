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
	o.color = v.color;
	return o;
}

[maxvertexcount(3)]   // produce a maximum of 3 output vertices
void GS( triangle VS_OUTPUT input[3], inout TriangleStream<VS_OUTPUT> triStream )
{
  PS_INPUT psInput;
  
  for( uint i = 0; i < 3; i++ )
  {
    psInput.Position = input[i].Position;
    psInput.Color = input[i].Color;
    triStream.Append(psInput);
  }
  triStream.RestartStrip();
}

[maxvertexcount(4)]
gsOutput gsMain(point vsOutput input[1], inout TriangleStream<gsOutput> output)
{
	gsOutput g;
	output.Append(g);
	output.RestartStrip();
}

float4 psMain(in gsOutput v) : SV_TARGET
{
	return v.color;
}
