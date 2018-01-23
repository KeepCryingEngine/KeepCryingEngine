#include "ModuleCamera.h"
#include "Application.h"

ModuleCamera::ModuleCamera()
{

}

ModuleCamera::~ModuleCamera()
{

}

bool ModuleCamera::Init()
{
	frustum.type = PerspectiveFrustum;
	frustum.pos = float3(0, 0, 0);
	frustum.front = float3(0, 0, 1);
	frustum.up = float3(0, 1, 0);
	frustum.nearPlaneDistance = 0.1;
	frustum.farPlaneDistance = 500;
	frustum.horizontalFov = pi/2;
	frustum.verticalFov = pi/2;
	return true;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

update_status ModuleCamera::Update(float deltaTimeS, float realDeltaTimeS)
{
	return update_status::UPDATE_CONTINUE;
}

float4x4 ModuleCamera::GetViewMatrix()const
{
	return float4x4(frustum.ViewMatrix().Transposed3());
}
float4x4 ModuleCamera::GetProyectionMatrix()const
{
	return frustum.ProjectionMatrix().Transposed();
}