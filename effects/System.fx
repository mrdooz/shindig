/*
	Effects used for debugging and profiling
*/

//////
// pos
//

cbuffer object
{
	float4x4 view_proj;
	float4 apa;
	float4 bepa;
}

cbuffer tjong
{
	float4 cepa;
	float4x4 view_proj2;
}

struct VS_POS_INPUT
{
    float3 Pos : POSITION;
};

struct PS_POS_INPUT
{
    float4 Pos : SV_POSITION;
};

PS_POS_INPUT VS_POS(VS_POS_INPUT input)
{
    PS_POS_INPUT output = (PS_POS_INPUT)0;
	output.Pos = mul(float4(input.Pos, 1), view_proj);
    return output;
}

float4 PS_POS(PS_POS_INPUT input) : SV_Target
{
    return float4(1, 1, 1, 1);
}

technique10 render_pos
{
    pass P0
    {
        VertexShader = compile vs_4_0 VS_POS();
		PixelShader = compile ps_4_0 PS_POS();
    }
}

//////
// pos normal
//

struct VS_POS_NORMAL_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
};


struct PS_POS_NORMAL_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : TEXTURE1;
};


//////
// pos color
//

struct VS_POS_COLOR_INPUT
{
    float3 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_POS_COLOR_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

PS_POS_COLOR_INPUT VS_POS_COLOR(VS_POS_COLOR_INPUT input)
{
    PS_POS_COLOR_INPUT output = (PS_POS_COLOR_INPUT)0;
    output.Pos = mul(float4(input.Pos, 1), view_proj) + mul(float4(input.Pos, 1), view_proj2);
    output.Color = input.Color;
    return output;
}

float4 PS_POS_COLOR(PS_POS_COLOR_INPUT input) : SV_Target
{
    return input.Color;
}

DepthStencilState dsNormal
{
    DepthEnable = false;
    DepthFunc = Less_Equal;
    DepthWriteMask = All;
    StencilEnable = false;
};

technique10 render_pos_color
{
    pass P0
    {
		VertexShader = compile vs_4_0 VS_POS_COLOR();
		PixelShader  = compile vs_4_0 PS_POS_COLOR();
    }
}

