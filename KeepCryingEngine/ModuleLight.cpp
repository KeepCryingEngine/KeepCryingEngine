#include "ModuleLight.h"

#include <Quat.h>
#include <MathFunc.h>

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleEditorUI.h"

const float3 LIGHT_POSITION_COLOR = float3 { 255, 255, 0 };
const float3 LIGHT_DIRECTION_COLOR = float3 { 255, 255, 0 };

ModuleLight::ModuleLight()
{ }

ModuleLight::~ModuleLight()
{ }

update_status ModuleLight::Update()
{
	if(App->uiEditor->GetDebugMode())
	{
		DrawLight();
	}

	return update_status::UPDATE_CONTINUE;
}

const float3& ModuleLight::GetPosition() const
{
	return currentPosition;
}

void ModuleLight::SetPosition(const float3& position)
{
	basePosition = position;
	currentPosition = position;
}

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

	Quat rotation = Quat::RotateY(DegToRad(360.0f * amount));

	currentPosition = rotation * basePosition;
	currentDirection = rotation * baseDirection;
}

void ModuleLight::DrawLight() const
{
	App->renderer->DrawSphere(currentPosition, LIGHT_POSITION_COLOR, 5.0f);
	App->renderer->DrawCross(currentPosition + 10.0f * currentDirection, LIGHT_DIRECTION_COLOR, 100.0f);
}