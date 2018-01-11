#ifndef _JSON_SERIALIZER_H_
#define _JSON_SERIALIZER_H_

#include "json.hpp"
using nlohmann::json;

class Vector3;
void to_json(json& j, const Vector3& v);
void from_json(const json& j, Vector3& v);

class Vector2;
void to_json(json& j, const Vector2& v);
void from_json(const json& j, Vector2& v);

#endif // !_JSON_SERIALIZER_H_