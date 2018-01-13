#include "json_serializer.h"
#include "json.hpp"
#include "Vector3.h"
#include "Vector2.h"

void to_json(json & j, const Vector3 & v) 
{
	j = json{ { "x",v.x },{ "y",v.y },{ "z",v.z } };
}

void from_json(const json & j, Vector3 & v) 
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
	
	auto z = j.find("z");
	if (z != j.end())
		v.z = z.value().get<float>();
}

void to_json(json & j, const Vector2 & v) 
{
	j = json{ { "x",v.x },{ "y",v.y } };
}

void from_json(const json & j, Vector2 & v) 
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
}