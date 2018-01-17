#include "ModuleTestMathGeoLib.h"

#include <MathGeoLib.h>

ModuleTestMathGeoLib::ModuleTestMathGeoLib()
{
}

ModuleTestMathGeoLib::~ModuleTestMathGeoLib()
{
}

bool ModuleTestMathGeoLib::Start()
{
	LOG_DEBUG("ModuleTestMathGeoLib");

	float3 float3(1.0f, 2.0f, 3.0f);

	LOG_DEBUG("float3: %s", float3.ToString().c_str());

	Sphere sphere(float3, 5.0f);

	LOG_DEBUG("sphere: %s", sphere.ToString().c_str());

	Quat quat(1.0f, 2.0f, 3.0f, 4.0f);

	LOG_DEBUG("quat: %s", quat.ToString().c_str());

	return true;
}