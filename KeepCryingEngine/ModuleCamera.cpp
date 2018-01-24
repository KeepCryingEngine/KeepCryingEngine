#include "ModuleCamera.h"
#include "Application.h"
#include "Globals.h"

const float ModuleCamera::SHIFT_MULTIPLIER = 10.0f;

ModuleCamera::ModuleCamera()
{ }

ModuleCamera::~ModuleCamera()
{ }

bool ModuleCamera::Init()
{
	SetUpFrustum();
	
	return true;
}

update_status ModuleCamera::Update(float deltaTimeS, float realDeltaTimeS)
{
	if(App->input->GetKey(SDL_SCANCODE_I))
	{
		SetUpFrustum();
	}

	float shiftDeltaMultiplier = deltaTimeS;

	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) || App->input->GetKey(SDL_SCANCODE_RSHIFT))
	{
		shiftDeltaMultiplier *= SHIFT_MULTIPLIER;
	}

	Rotation(shiftDeltaMultiplier);

	Movement(shiftDeltaMultiplier);

	return update_status::UPDATE_CONTINUE;
}

void ModuleCamera::SetMoveSpeed(float speed)
{
	movementSpeed = speed;
}

void ModuleCamera::SetRotationSpeed(float speed)
{
	rotationSpeed = speed;
}

void ModuleCamera::SetDragSpeed(float speed)
{
	movementDragSpeed = speed;
}

void ModuleCamera::SetOrbitSpeed(float speed)
{
	movementOrbitSpeed = speed;
}

void ModuleCamera::SetZoomSpeed(float speed)
{
	movementZoomSpeed = speed;
}

void ModuleCamera::SetFOV(float radians)
{
	frustum.horizontalFov = ComputeHorizontalFov(radians, (float)App->window->width, (float)App->window->height);
	frustum.verticalFov = radians;
}

void ModuleCamera::SetAspectRatio()
{
	frustum.horizontalFov = ComputeHorizontalFov(frustum.verticalFov, (float)App->window->width, (float)App->window->height);
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

float ModuleCamera::GetMoveSpeed() const
{
	return movementSpeed;
}

float ModuleCamera::GetRotationSpeed() const
{
	return rotationSpeed;
}

float ModuleCamera::GetDragSpeed() const
{
	return movementDragSpeed;
}

float ModuleCamera::GetOrbitSpeed() const
{
	return movementOrbitSpeed;
}

float ModuleCamera::GetZoomSpeed() const
{
	return movementZoomSpeed;
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

float3 ModuleCamera::GetFrontVector() const
{
	return frustum.front;
}

float3 ModuleCamera::GetUpVector() const
{
	return frustum.up;
}

float3 ModuleCamera::GetSideVector() const
{
	return Cross(frustum.front, frustum.up);
}

void ModuleCamera::LookAt(float3 point)
{
	float3 lookAtDirectionNormalized = (point - frustum.pos).Normalized();

	Quat lookAtRotation = Quat::LookAt(
		frustum.front,
		lookAtDirectionNormalized,
		frustum.up,
		float3::unitY
	);

	frustum.front = lookAtRotation.Mul(frustum.front);
	frustum.up = lookAtRotation.Mul(frustum.up);
}

float4x4 ModuleCamera::GetViewMatrix()const
{
	return float4x4(frustum.ViewMatrix()).Transposed();
}

float4x4 ModuleCamera::GetProyectionMatrix()const
{
	return frustum.ProjectionMatrix().Transposed();
}

void ModuleCamera::SetUpFrustum()
{
	frustum.type = PerspectiveFrustum;
	frustum.pos = float3::unitY;
	frustum.front = -float3::unitZ;
	frustum.up = float3::unitY;
	frustum.nearPlaneDistance = 0.1f;
	frustum.farPlaneDistance = 50.0f;
	frustum.verticalFov = DegToRad(60.0f);
	frustum.horizontalFov = ComputeHorizontalFov(frustum.verticalFov, SCREEN_WIDTH, SCREEN_HEIGHT);
}

float ModuleCamera::ComputeHorizontalFov(float radians, float width, float height) const
{
	return 2.0f * atan(tan(radians / 2.0f) * (width / height));
}

void ModuleCamera::Rotation(float shiftDeltaMultiplier)
{
	Quat rotation = Quat::identity;
	float rotationDeltaSpeed = rotationSpeed * shiftDeltaMultiplier;

	if(App->input->GetKey(SDL_SCANCODE_UP))
	{
		if(frustum.front.y < 1.0f - rotationDeltaSpeed)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(frustum.WorldRight(), rotationDeltaSpeed));
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_DOWN))
	{
		if(frustum.front.y > rotationDeltaSpeed - 1.0f)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(frustum.WorldRight(), -rotationDeltaSpeed));
		}
	}

	if(App->input->GetKey(SDL_SCANCODE_LEFT))
	{
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, rotationDeltaSpeed));
	}

	if(App->input->GetKey(SDL_SCANCODE_RIGHT))
	{
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, -rotationDeltaSpeed));
	}

	frustum.up = rotation.Mul(frustum.up);
	frustum.front = rotation.Mul(frustum.front);
}

void ModuleCamera::Movement(float shiftDeltaMultiplier)
{
	MovementMouse(shiftDeltaMultiplier);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		MovementKeyBoard(shiftDeltaMultiplier);
	}
}

void ModuleCamera::MovementMouse(float shiftDeltaMultiplier)
{
	bool leftPressed = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);
	bool rightPressed = App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT);
	bool altPressed = App->input->GetKey(SDL_SCANCODE_LALT) || App->input->GetKey(SDL_SCANCODE_RALT);

	if(altPressed)
	{
		if(leftPressed)
		{
			MovementMouseOrbit(shiftDeltaMultiplier);
		}
		else if(rightPressed)
		{
			MovementMouseZoom(shiftDeltaMultiplier);
		}
	}
	else if(leftPressed)
	{
		MovementMouseDrag(shiftDeltaMultiplier);
	}
}

void ModuleCamera::MovementMouseDrag(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector *= movementDragSpeed * shiftDeltaMultiplier;

	translateVector.x *= -1.0f;

	float3 upMovement = translateVector.y * frustum.up;
	float3 rightMovement = translateVector.x * frustum.WorldRight();

	frustum.Translate(upMovement + rightMovement);
}

void ModuleCamera::MovementMouseOrbit(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector *= movementDragSpeed * shiftDeltaMultiplier;

	translateVector.x *= -1.0f;

	Quat rotation = Quat::identity;
	float movementDeltaOrbitSpeed = movementOrbitSpeed * shiftDeltaMultiplier;

	if(translateVector.y > 0.0f)
	{
		if(frustum.front.y < 1.0f - movementDeltaOrbitSpeed)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(translateVector.y * frustum.WorldRight(), movementDeltaOrbitSpeed));
		}
	}
	else if(translateVector.y < 0.0f)
	{
		if(frustum.front.y > movementDeltaOrbitSpeed - 1.0f)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(translateVector.y * frustum.WorldRight(), movementDeltaOrbitSpeed));
		}
	}

	rotation = rotation.Mul(Quat::RotateAxisAngle(translateVector.x * float3::unitY, movementDeltaOrbitSpeed));

	frustum.up = rotation.Mul(frustum.up);
	frustum.front = rotation.Mul(frustum.front);
}

void ModuleCamera::MovementMouseZoom(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector.x *= -1.0f;

	float movementZ = translateVector.y - translateVector.x;

	frustum.Translate(frustum.front * movementZ * shiftDeltaMultiplier);
}

void ModuleCamera::MovementKeyBoard(float shiftDeltaMultiplier)
{
	float3 translateVector = float3::zero;

	if(App->input->GetKey(SDL_SCANCODE_Q))
	{
		translateVector += frustum.up;
	}

	if(App->input->GetKey(SDL_SCANCODE_E))
	{
		translateVector -= frustum.up;
	}

	if(App->input->GetKey(SDL_SCANCODE_W))
	{
		translateVector += frustum.front;
	}

	if(App->input->GetKey(SDL_SCANCODE_S))
	{
		translateVector -= frustum.front;
	}

	if(App->input->GetKey(SDL_SCANCODE_D))
	{
		translateVector += frustum.WorldRight();
	}

	if(App->input->GetKey(SDL_SCANCODE_A))
	{
		translateVector -= frustum.WorldRight();
	}

	frustum.Translate(translateVector * movementSpeed * shiftDeltaMultiplier);
}