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
	frustum.farPlaneDistance = 50;
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
	float fovH = 2 * atan(tan(frustum.verticalFov / 2)*aspect);
	frustum.horizontalFov = fovH;
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
float ModuleCamera::GetFOV() const
{
	return frustum.verticalFov;
}
float ModuleCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}
float ModuleCamera::GetNearPlane() const
{
	return frustum.nearPlaneDistance;
}
float ModuleCamera::GetFarPlane() const
{
	return frustum.farPlaneDistance;
}
float3 ModuleCamera::GetPosition() const
{
	return frustum.pos;
}

float3 ModuleCamera::Orientation()
{
	return frustum.front;
}
void ModuleCamera::LookAt(float3 point)
{
	float3 forward_camera = (point - frustum.pos).Normalized();
	float3 left_camera = Cross( float3(0,1,0),forward_camera).Normalized();
	float3 up_camera = Cross(forward_camera,left_camera).Normalized();
	frustum.front = forward_camera;
	frustum.up = up_camera;

}

float4x4 ModuleCamera::GetViewMatrix()const
{
	return float4x4(frustum.ViewMatrix().Transposed3());
}
float4x4 ModuleCamera::GetProyectionMatrix()const
{
	return frustum.ProjectionMatrix().Transposed();
}