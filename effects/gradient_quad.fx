// Shader that just outputs a full screen quad

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

/*

// Note, this isn't valid anymore, because we're using a triangle strip to
// work with pre dx 10 hardware
// 0-1
// 2-3

float4 c0;
float4 c1;
float4 c2;
float4 c3;

psInput vsMain(in uint v : SV_VertexID)
{
	psInput o = (psInput)0;
	if (v == 0) {
		o.pos = float4(-1, +1, 0, 1);
		o.col = c0;
	} else if (v == 1) {
		o.pos = float4(+1, +1, 0, 1);
		o.col = c1;
	} else if (v == 2) {
		o.pos = float4(-1, -1, 0, 1);
		o.col = c2;
	} else if (v == 3) {
		o.pos = float4(+1, -1, 0, 1);
		o.col = c3;
	}
	return o;
}
*/

psInput vsMain(in vsInput v)
{
	psInput o = (psInput)0;
	o.pos = v.pos;
	o.col = v.col;
	return o;
}

float4 psMain(in psInput v) : SV_Target
{
	return v.col;
}
