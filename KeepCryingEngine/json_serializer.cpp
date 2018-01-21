#include "json_serializer.h"

#include "json.hpp"

void to_json(json & j, const float3 & v)
{
	j = json{ { "x",v.x },{ "y",v.y },{ "z",v.z } };
}

void from_json(const json & j, float3 & v)
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();

	auto z = j.find("z");
	if(z != j.end())
	{
		v.z = z.value().get<float>();
	}
}