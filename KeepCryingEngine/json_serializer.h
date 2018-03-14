#ifndef _JSON_SERIALIZER_H_
#define _JSON_SERIALIZER_H_

#include <Quat.h>
#include <float3.h>
#include <json.hpp>
#include <Frustum.h>
#include <set>
#include <bass.h>
#include <experimental/filesystem>

void to_json(nlohmann::json& j, const float2& v);
void from_json(const nlohmann::json& j, float2& v);

void to_json(nlohmann::json& j, const float3& v);
void from_json(const nlohmann::json& j, float3& v);

void to_json(nlohmann::json& j, const Quat& v);
void from_json(const nlohmann::json& j, Quat& v);

void to_json(nlohmann::json& j, const Frustum& v);
void from_json(const nlohmann::json& j, Frustum& v);

void to_json(nlohmann::json& j, const std::set<std::experimental::filesystem::path>& s);
void from_json(const nlohmann::json& j, std::set<std::experimental::filesystem::path>& s);

void to_json(nlohmann::json& j, const BASS_DX8_I3DL2REVERB& v);
void from_json(const nlohmann::json& j, BASS_DX8_I3DL2REVERB& v);

#endif // !_JSON_SERIALIZER_H_