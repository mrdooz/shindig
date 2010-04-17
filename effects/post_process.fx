texture2d g_MeshTexture;              // Color texture for mesh

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
	vsOutput o = (vsOutput)0;
	o.position = float4(v.position, 1.0f);	
	o.color = v.color;
	return o;
}

float4 psMain(vsOutput input) : SV_Target
{
    return input.color;
}
