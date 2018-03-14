#include "json_serializer.h"

using nlohmann::json;

void to_json(nlohmann::json & j, const float2 & v)
{
	j = json{ { "x", v.x },{ "y", v.y } };
}

void from_json(const nlohmann::json & j, float2 & v)
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
}

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

void to_json(nlohmann::json& j, const float4& v)
{
	j = json{ { "x", v.x },{ "y", v.y },{ "z", v.z },{ "w", v.w } };
}

void from_json(const nlohmann::json& j, float4& v)
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
	v.z = j.at("z").get<float>();
	v.w = j.at("w").get<float>();
}

void to_json(nlohmann::json& j, const Quat& v)
{
	j = json{ { "x", v.x },{ "y", v.y },{ "z", v.z },{ "w", v.w } };
}

void from_json(const nlohmann::json& j, Quat& v)
{
	v.x = j.at("x").get<float>();
	v.y = j.at("y").get<float>();
	v.z = j.at("z").get<float>();
	v.w = j.at("w").get<float>();
}

void to_json(nlohmann::json& j, const Frustum& v)
{
	nlohmann::json jsonPos;
	to_json(jsonPos, v.pos);

	nlohmann::json jsonUp;
	to_json(jsonUp, v.up);

	nlohmann::json jsonFront;
	to_json(jsonFront, v.front);

	j = json
	{
		{ "pos", jsonPos },
		{ "type", v.type },
		{ "up", jsonUp },
		{ "front", jsonFront },
		{ "verticalFov", v.verticalFov },
		{ "horizontalFov", v.horizontalFov },
		{ "farPlaneDistance", v.farPlaneDistance },
		{ "nearPlaneDistance", v.nearPlaneDistance },
		{ "orthographicWidth", v.orthographicWidth },
		{ "orthographicHeight", v.orthographicHeight }
	};
}

void from_json(const nlohmann::json& j, Frustum& v)
{
	float3 pos;
	from_json(j["pos"], pos);

	float3 up;
	from_json(j["up"], up);

	float3 front;
	from_json(j["front"], front);

	v.pos = pos;
	v.type = j["type"];
	v.up = up;
	v.front = front;
	v.verticalFov = j["verticalFov"];
	v.horizontalFov = j["horizontalFov"];
	v.farPlaneDistance = j["farPlaneDistance"];
	v.nearPlaneDistance = j["nearPlaneDistance"];
	v.orthographicWidth = j["orthographicWidth"];
	v.orthographicHeight = j["orthographicHeight"];
}

void to_json(nlohmann::json& j, const BASS_DX8_I3DL2REVERB& v)
{
	j = json
	{
		{ "lRoom", v.lRoom },
		{ "flRoomRolloffFactor", v.flRoomRolloffFactor },
		{ "flDecayTime", v.flDecayTime },
		{ "flDecayHFRatio", v.flDecayHFRatio },
		{ "lReflections", v.lReflections },
		{ "flReflectionsDelay", v.flReflectionsDelay },
		{ "lReverb", v.lReverb },
		{ "flReverbDelay", v.flReverbDelay },
		{ "flDiffusion", v.flDiffusion },
		{ "flDensity", v.flDensity },
		{ "flHFReference", v.flHFReference }
	};
}

void from_json(const nlohmann::json& j, BASS_DX8_I3DL2REVERB& v)
{
	v.lRoom = j["lRoom"];
	v.flRoomRolloffFactor = j["flRoomRolloffFactor"];
	v.flDecayTime = j["flDecayTime"];
	v.flDecayHFRatio = j["flDecayHFRatio"];
	v.lReflections = j["lReflections"];
	v.flReflectionsDelay = j["flReflectionsDelay"];
	v.lReverb = j["lReverb"];
	v.flReverbDelay = j["flReverbDelay"];
	v.flDiffusion = j["flDiffusion"];
	v.flDensity = j["flDensity"];
	v.flHFReference = j["flHFReference"];
}