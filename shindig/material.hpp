#pragma once

struct Material
{

	typedef std::map<string2, float> FloatValues;
	typedef std::map<string2, float> Float2Values;
	typedef std::map<string2, float> Float3Values;
	typedef std::map<string2, float> Float4Values;
	typedef std::map<string2, string2> StringValues;

	FloatValues float_values;
	Float2Values float2_values;
	Float3Values float3_values;
	Float4Values float4_values;

	string2 name;
};