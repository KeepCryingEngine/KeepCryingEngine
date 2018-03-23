#include "ModuleCamera.h"

#include "Camera.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleEditorUI.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "Transform.h"

const float ModuleCamera::SHIFT_MULTIPLIER = 10.0f;
const float ModuleCamera::WHEEL_FORCE = 10.0f;
const float ModuleCamera::ORBIT_FORCE_REDUCTION = 10.0f;

ModuleCamera::ModuleCamera()
{ }

ModuleCamera::~ModuleCamera()
{ }

bool ModuleCamera::Init()
{
	gameObjectCamera = new GameObject("Editor Camera");
	camera = gameObjectCamera->AddComponent<Camera>();
	camera->SetUpFrustum(float3(0, 1, -10), Quat::identity, 0.1f, 300.0f);

	return true;
}

update_status ModuleCamera::Update()
{
	/*if(App->input->GetKey(SDL_SCANCODE_I) == KeyState::KEY_DOWN)
	{
		camera->SetUpFrustum(float3(0, 1, -10), Quat::identity);
	}*/

	float shiftDeltaMultiplier = App->time->GetEditorDeltaTime();

	if(App->input->GetKeyPressed(SDL_SCANCODE_LSHIFT) || App->input->GetKeyPressed(SDL_SCANCODE_RSHIFT))
	{
		shiftDeltaMultiplier *= SHIFT_MULTIPLIER;
	}

	Rotation(App->time->GetEditorDeltaTime());

	Movement(shiftDeltaMultiplier);

	return update_status::UPDATE_CONTINUE;
}

bool ModuleCamera::CleanUp()
{
	RELEASE(camera);

	return true;
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

void ModuleCamera::Rotation(float deltaTimeS)
{
	RotateKeyboard(deltaTimeS);
	RotateMouse(deltaTimeS);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) && !(App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT)))
	{
		RotateMouseRotation(deltaTimeS);
	}
	
}

void ModuleCamera::Movement(float shiftDeltaMultiplier)
{
	MovementMouse(shiftDeltaMultiplier);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		MovementKeyBoard(shiftDeltaMultiplier);
	}
}

void ModuleCamera::RotateMouse(float deltaTimeS)
{
	bool leftPressed = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);
	bool altPressed = App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT);

	if(altPressed)
	{
		if(leftPressed)
		{
			RotateMouseOrbit(deltaTimeS);
		}
	}
}

void ModuleCamera::MovementMouse(float shiftDeltaMultiplier)
{
	bool leftPressed = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);
	bool rightPressed = App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT);
	bool altPressed = App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT);
	float wheelMotion = App->input->GetWheelMotion();

	if(wheelMotion == 1)
	{
		MovementWheelZoom(shiftDeltaMultiplier*WHEEL_FORCE);
	}
	if(wheelMotion == -1)
	{
		MovementWheelZoom(-shiftDeltaMultiplier*WHEEL_FORCE);
	}

	if(altPressed)
	{
		if(rightPressed && wheelMotion == 0)
		{
			MovementMouseZoom(shiftDeltaMultiplier);
		}
	}
	else if(leftPressed)
	{
		switch(App->uiEditor->GetClickMode())
		{
			case ClickMode::Drag:
			{
				MovementMouseDrag(shiftDeltaMultiplier);
			}
			break;
			case ClickMode::Pick:
			{
				ScenePick();
			}
			break;
		}
	}
}

void ModuleCamera::MovementMouseDrag(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector *= movementDragSpeed * shiftDeltaMultiplier;

	translateVector.x *= -1.0f;

	float3 upMovement = translateVector.y * gameObjectCamera->GetTransform()->Up(); // frustum.up;
	float3 rightMovement = translateVector.x * gameObjectCamera->GetTransform()->Right(); // frustum.WorldRight();

	gameObjectCamera->GetTransform()->Translate(upMovement + rightMovement);
}

void ModuleCamera::RotateMouseOrbit(float deltaTimeS)
{
	//It rotate around a point "x" at distance "y" in front direction, "y" value increases with zoom out, and decreases with zoom in
	if(zoomAmount > 1.0f)
	{
		float3 orbitCenter = gameObjectCamera->GetTransform()->GetLocalPosition() + (gameObjectCamera->GetTransform()->Forward() * zoomAmount);
		App->renderer->DrawCross(orbitCenter, zoomAmount);
		MovementMouseDrag(deltaTimeS * zoomAmount / ORBIT_FORCE_REDUCTION);
		gameObjectCamera->GetTransform()->LookAt(orbitCenter);
	}
	else
	{
		RotateMouseRotation(deltaTimeS);
	}
}

void ModuleCamera::MovementMouseZoom(float shiftDeltaMultiplier)
{
	float2 translateVector = App->input->GetMouseMotion();

	translateVector.x *= -1.0f;

	float movementZ = translateVector.y - translateVector.x;

	float movementTemp = movementZoomSpeed * movementZ * shiftDeltaMultiplier;
	zoomAmount -= movementTemp;
	if(zoomAmount >= 0.0f)
	{
		gameObjectCamera->GetTransform()->Translate(gameObjectCamera->GetTransform()->Forward() * movementTemp);
	}
	else
	{
		zoomAmount = 0.0f;
	}
}

void ModuleCamera::MovementWheelZoom(float shiftDeltaMultiplier)
{
	float movementTemp = movementZoomSpeed * shiftDeltaMultiplier;
	zoomAmount -= movementTemp;

	if(zoomAmount >= 0.0f)
	{
		gameObjectCamera->GetTransform()->Translate(gameObjectCamera->GetTransform()->Forward() * movementTemp);
	}
	else
	{
		zoomAmount = 0.0f;
	}
}

void ModuleCamera::RotateMouseRotation(float deltaTimeS)
{
	float2 motionVector = App->input->GetMouseMotion();

	motionVector *= movementDragSpeed * deltaTimeS;

	motionVector.x *= -1.0f;

	Quat rotation = gameObjectCamera->GetTransform()->GetLocalRotation();
	float movementDeltaOrbitSpeed = movementOrbitSpeed * deltaTimeS;

	if(motionVector.y > 0.0f)
	{
		if(gameObjectCamera->GetTransform()->Forward().y < 1.0f - movementDeltaOrbitSpeed)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(motionVector.y * gameObjectCamera->GetTransform()->Right(), movementDeltaOrbitSpeed));
		}
	}
	else if(motionVector.y < 0.0f)
	{
		if(gameObjectCamera->GetTransform()->Forward().y > movementDeltaOrbitSpeed - 1.0f)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(motionVector.y * gameObjectCamera->GetTransform()->Right(), movementDeltaOrbitSpeed));
		}
	}

	rotation = rotation.Mul(Quat::RotateAxisAngle(motionVector.x * float3::unitY, movementDeltaOrbitSpeed));


	gameObjectCamera->GetTransform()->SetLocalRotation(rotation.Normalized());
}

void ModuleCamera::MovementKeyBoard(float shiftDeltaMultiplier)
{
	float3 translateVector = float3::zero;

	if(App->input->GetKeyPressed(SDL_SCANCODE_Q))
	{
		translateVector += gameObjectCamera->GetTransform()->Up();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_E))
	{
		translateVector -= gameObjectCamera->GetTransform()->Up();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_W))
	{
		translateVector += gameObjectCamera->GetTransform()->Forward();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_S))
	{
		translateVector -= gameObjectCamera->GetTransform()->Forward();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_D))
	{
		translateVector += gameObjectCamera->GetTransform()->Right();
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_A))
	{
		translateVector -= gameObjectCamera->GetTransform()->Right();
	}

	gameObjectCamera->GetTransform()->Translate(translateVector * movementSpeed * shiftDeltaMultiplier);
}

void ModuleCamera::RotateKeyboard(float deltaTimeS)
{
	Quat rotation = gameObjectCamera->GetTransform()->GetLocalRotation();
	float rotationDeltaSpeed = rotationSpeed * deltaTimeS;

	if(App->input->GetKeyPressed(SDL_SCANCODE_UP))
	{
		if(gameObjectCamera->GetTransform()->Forward().y < 1.0f - rotationDeltaSpeed)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(gameObjectCamera->GetTransform()->Right(), rotationDeltaSpeed));
		}
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_DOWN))
	{
		if(gameObjectCamera->GetTransform()->Forward().y > rotationDeltaSpeed - 1.0f)
		{
			rotation = rotation.Mul(Quat::RotateAxisAngle(gameObjectCamera->GetTransform()->Right(), -rotationDeltaSpeed));
		}
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_LEFT))
	{
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, rotationDeltaSpeed));
	}

	if(App->input->GetKeyPressed(SDL_SCANCODE_RIGHT))
	{
		rotation = rotation.Mul(Quat::RotateAxisAngle(float3::unitY, -rotationDeltaSpeed));
	}

	gameObjectCamera->GetTransform()->SetLocalRotation(rotation.Normalized());
}

void ModuleCamera::ScenePick()
{
	float2 mousePos = App->input->GetMousePosition();
	float normalizedX = ((mousePos.x/ App->configuration.screenWidth)*2) - 1 ;
	float normalizedY =((mousePos.y / App->configuration.screenHeight)*-2)+1;

	LineSegment picking = camera->GetFrustum().UnProjectLineSegment(normalizedX, normalizedY);
	lastRay = picking;

	RayCastHit hit;

	if(App->scene->RayCast(picking.a, picking.Dir(), camera->GetFarPlane(), hit))
	{
		App->uiEditor->OpenInspectorWindow();
		App->uiEditor->SetSelectedNodeID(hit.gameObject->GetId());
	}
	else
	{
		App->uiEditor->CloseInspectorWindow();
		App->uiEditor->SetSelectedNodeID(0);
	}
}

void ModuleCamera::EnableCamera(Camera* camera)
{
	if(enabledCamera != nullptr)
	{
		enabledCamera->enabled = false;
	}

	if(camera != nullptr)
	{
		camera->enabled = true;
	}

	enabledCamera = camera;
}

Camera* ModuleCamera::GetEnabledCamera() const
{
	return enabledCamera;
}

const LineSegment & ModuleCamera::GetLastRay() const
{
	return lastRay;
}
