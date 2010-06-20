#pragma once

struct Material
{
	Material(const string2& name);

	typedef std::map<string2, float> FloatValues;
	typedef std::map<string2, D3DXVECTOR2> Float2Values;
	typedef std::map<string2, D3DXVECTOR3> Float3Values;
	typedef std::map<string2, D3DXVECTOR4> Float4Values;
	typedef std::map<string2, string2> StringValues;

	FloatValues float_values;
	Float2Values float2_values;
	Float3Values float3_values;
	Float4Values float4_values;
	StringValues string_values;

	string2 name;
};