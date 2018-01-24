#include "ModuleCamera.h"
#include "Application.h"
#include "Globals.h"

const float ModuleCamera::SHIFT_MOVEMENT_MULTIPLIER = 10.0f;

ModuleCamera::ModuleCamera()
{ }

ModuleCamera::~ModuleCamera()
{ }

bool ModuleCamera::Init()
{
	frustum.type = PerspectiveFrustum;
	frustum.pos = float3(0, 1, 0);
	frustum.front = float3(0, 0, -1);
	frustum.up = float3(0, 1, 0);
	frustum.nearPlaneDistance = 0.1;
	frustum.farPlaneDistance = 50;
	frustum.verticalFov = DegToRad(60.0f); // 1.0472f; //60 degrees in radians
	frustum.horizontalFov = 2.0f * atan(tan(frustum.verticalFov / 2.0f)*((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT));
	
	return true;
}

update_status ModuleCamera::Update(float deltaTimeS, float realDeltaTimeS)
{
	if(App->input->GetKey(SDL_SCANCODE_I))
	{
		Init(); // Jaja
	}

	//Rotation
	// float3x3 rotationMatrix;
	// rotationMatrix.SetIdentity();
	Quat rotation = Quat::identity;

	if(App->input->GetKey(SDL_SCANCODE_UP))
	{
		/*float deltaTimeRotation = rotationSpeed * deltaTimeS * 0.5f;
		float angleBetween = frustum.front.AngleBetweenNorm(float3::unitY);
		float rotationAngle = min(deltaTimeRotation, angleBetween);
		rotation = rotation.Mul(Quat::RotateAxisAngle(frustum.WorldRight(), rotationAngle));
		*/// frustum.Transform(rotation);
		// frustum.up = rotation.Mul(frustum.up);
		// frustum.front = rotation.Mul(frustum.front);

		if(frustum.front.y < 1 - 0.1f*rotationSpeed * deltaTimeS)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(frustum.WorldRight(), 0.1f*rotationSpeed * deltaTimeS));
		}

		/*if(frustum.front.y < 1 - 0.1f*rotationSpeed * deltaTimeS)
		{
			float timeBasedSpeed = 0.1f*rotationSpeed * deltaTimeS;
			// Quat rotation = Quat::RotateAxisAngle(frustum.WorldRight(), timeBasedSpeed);
			Quat rotation = Quat::RotateX(0.1f*rotationSpeed * deltaTimeS);
			frustum.Transform(rotation);
		}*/
	}
	if(App->input->GetKey(SDL_SCANCODE_DOWN))
	{
		if(frustum.front.y > -1 + 0.1f*rotationSpeed * deltaTimeS)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(frustum.WorldRight(), -0.1f*rotationSpeed * deltaTimeS));
		}
	}
	if(App->input->GetKey(SDL_SCANCODE_LEFT))
	{
		float timeBasedSpeed = rotationSpeed * deltaTimeS;
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, timeBasedSpeed));
		// frustum.Transform(rotation);
	}
	if(App->input->GetKey(SDL_SCANCODE_RIGHT))
	{
		float timeBasedSpeed = -rotationSpeed * deltaTimeS;
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, timeBasedSpeed));
	}

	frustum.up = rotation.Mul(frustum.up);
	frustum.front = rotation.Mul(frustum.front);

	// Movement
	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		float speedDelta = movementSpeed * deltaTimeS;

		if(App->input->GetKey(SDL_SCANCODE_LSHIFT) || App->input->GetKey(SDL_SCANCODE_RSHIFT))
		{
			speedDelta *= SHIFT_MOVEMENT_MULTIPLIER;
		}

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

		frustum.Translate(translateVector * speedDelta);
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleCamera::SetFOV(float radians)
{
	float fovH = 2.0f * atan(tan(radians / 2.0f)*((float)App->window->width / (float)App->window->height));
	frustum.horizontalFov = fovH;
	frustum.verticalFov = radians;
}

void ModuleCamera::SetAspectRatio()
{
	float fovH = 2 * atan(tan(frustum.verticalFov / 2)*((float)App->window->width/(float)App->window->height));
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

float3 ModuleCamera::Orientation() const
{
	return frustum.front;
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