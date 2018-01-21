#ifndef _JSON_SERIALIZER_H_
#define _JSON_SERIALIZER_H_

#include <MathGeoLib.h>

#include "json.hpp"
using nlohmann::json;

void to_json(json& j, const float3& v);
void from_json(const json& j, float3& v);

#endif // !_JSON_SERIALIZER_H_