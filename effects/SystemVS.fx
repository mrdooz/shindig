cbuffer main
{
	matrix mWorldViewProj;	
};

struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT vsMain( in VS_INPUT v )
{
	VS_OUTPUT o = (VS_OUTPUT)0;
	
	o.position = mul( float4( v.position, 1.0f ), mWorldViewProj );	
	o.color = v.color;		
	return o;
}

