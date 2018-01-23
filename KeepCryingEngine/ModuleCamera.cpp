#include "ModuleCamera.h"
#include "Application.h"
#include "Globals.h"


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

void ModuleCamera::SetFOV(float radians)
{
	float fovH = 2 * atan(tan(radians / 2)*(SCREEN_WIDTH / SCREEN_HEIGHT));
	frustum.horizontalFov = fovH;
	frustum.verticalFov = radians;
}
void ModuleCamera::SetAspectRatio(float aspect)
{
	float fovH = 2 * atan(tan(radians / 2)*(SCREEN_WIDTH / SCREEN_HEIGHT));
}
void ModuleCamera::SetNearPlane(float distance)
{
	frustum.nearPlaneDistance = distance;
}
void ModuleCamera::SetFarPlane(float distance)
{
	frustum.farPlaneDistance = distance;
}
void ModuleCamera::SetPosition(float3 position)
{
	frustum.pos = position;
}
void ModuleCamera::SetOrientation(float3 rotation)
{

}
void ModuleCamera::LookAt(float3 point)
{

}

float4x4 ModuleCamera::GetViewMatrix()const
{
	return float4x4(frustum.ViewMatrix().Transposed3());
}
float4x4 ModuleCamera::GetProyectionMatrix()const
{
	return frustum.ProjectionMatrix().Transposed();
}