#ifndef _JSON_SERIALIZER_H_
#define _JSON_SERIALIZER_H_

#include <Quat.h>
#include <float3.h>
#include <json.hpp>
#include <Frustum.h>

void to_json(nlohmann::json& j, const float3& v);
void from_json(const nlohmann::json& j, float3& v);

void to_json(nlohmann::json& j, const Quat& v);
void from_json(const nlohmann::json& j, Quat& v);

void to_json(nlohmann::json& j, const Frustum& v);
void from_json(const nlohmann::json& j, Frustum& v);

#endif // !_JSON_SERIALIZER_H_