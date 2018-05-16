#include "ModuleLight.h"

#include <Quat.h>
#include <MathFunc.h>

ModuleLight::ModuleLight()
{ }

ModuleLight::~ModuleLight()
{ }

const float3& ModuleLight::GetDirection() const
{
	return currentDirection;
}

void ModuleLight::SetDirection(const float3& direction)
{
	baseDirection = direction;
	currentDirection = direction;
}

void ModuleLight::SetRotation(float amount)
{
	// amount 0 .. 1
	// amount 0 .. 360

	currentDirection = Quat::RotateY(DegToRad(360.0f * amount)) * baseDirection;
}