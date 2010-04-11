cbuffer main
{
	matrix mWorldViewProj;	
};

struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 t : TEXCOORD;
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
	o.color = float4(1,1,1,1);
	return o;
}

float4 psMain( in VS_OUTPUT p) : SV_Target
{
	return float4(1,1,1,1);
}