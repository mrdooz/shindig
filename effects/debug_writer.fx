// Shader that just outputs a full screen quad

struct psInput
{
	float4	pos : SV_Position;
	float4	col : Color;
};

// Note, this isn't valid anymore, because we're using a triangle strip to
// work with pre dx 10 hardware
// 0-1
// 2-3

psInput vsMain(in uint v : SV_VertexID)
{
	psInput o = (psInput)0;
	if (v == 0) {
		o.pos = float4(-1, +1, 0, 1);
		o.col = float4(1,0,0,1);
	} else if (v == 1) {
		o.pos = float4(+1, +1, 0, 1);
		o.col = float4(1,1,0,1);
	} else if (v == 2) {
		o.pos = float4(-1, -1, 0, 1);
		o.col = float4(1,0,1,1);
	} else if (v == 3) {
		o.pos = float4(+1, -1, 0, 1);
		o.col = float4(1,1,1,1);
	}
	return o;
}

float4 psMain(in psInput v) : SV_Target
{
	return v.col;
}
