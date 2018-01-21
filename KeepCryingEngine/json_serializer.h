#ifndef _JSON_SERIALIZER_H_
#define _JSON_SERIALIZER_H_

#include <float3.h>

#include "json.hpp"

void to_json(nlohmann::json& j, const float3& v);
void from_json(const nlohmann::json& j, float3& v);

#endif // !_JSON_SERIALIZER_H_