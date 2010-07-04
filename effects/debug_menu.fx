// draws a colored triangle.
// coordinates are in clip space
struct psInput
{
	float4	pos : SV_Position;
	float4	col : Color;
};

struct vsInput
{
	float4	pos : SV_Position;
	float4	col : Color;
};

psInput vsMain(in vsInput v)
{
	psInput o = (psInput)0;
	o.pos = v.pos;
	o.col = v.col;
	return o;
}

float4 psMain(in psInput v) : SV_Target
{
	return float4(1,1,1,1);
	return v.col;
}
