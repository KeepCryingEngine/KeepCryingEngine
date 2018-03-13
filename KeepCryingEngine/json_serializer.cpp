#include "json_serializer.h"

using nlohmann::json;

void to_json(json& j, const float3& v)
{
	j = json{ { "x", v.x },{ "y", v.y },{ "z", v.z } };
}

void from_json(const json& j, float3& v)
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
	v.z = j.at("z").get<float>();
}