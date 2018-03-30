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
const float ModuleCamera::KEYBOARD_ROTATION_SPEED_MULTIPLIER = 4;

ModuleCamera::ModuleCamera()
{ }

ModuleCamera::~ModuleCamera()
{ }

bool ModuleCamera::Init()
{
	cameraGameObject = new GameObject("Editor Camera");
	cameraTransform = cameraGameObject->GetComponent<Transform>();
	camera = cameraGameObject->AddComponent<Camera>();
	camera->SetIgnoreFrustumRendering(true);

	//init camera
	cameraTransform->SetWorldPosition(float3(0, 1, -10));
	cameraTransform->SetWorldRotation(Quat::FromEulerXYZ(0, 0, 0));
	camera->SetUpCamera(0.1f, 300.0f, 60.0f);

	return true;
}

update_status ModuleCamera::Update()
{
	Rotation();
	Orbit();
	Movement();

	camera->Update();

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

void ModuleCamera::Rotation()
{
	RotationMouse();
	RotationKeyboard();
}

void ModuleCamera::Movement()
{	
	MovementMouse();
	MovementKeyBoard();
}

void ModuleCamera::Orbit()
{
	bool leftPressed = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);
	bool altPressed = App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT);

	if(altPressed && leftPressed)
	{
		OrbitMouse();
	}
}

void ModuleCamera::MovementMouse()
{
	float shiftDeltaMultiplier = CalculateShiftDeltaMultiplier();
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
	float2 input = App->input->GetMouseMotion();
	float2 translateVector = input * movementDragSpeed * shiftDeltaMultiplier;

	float3 upMovement = translateVector.y * cameraTransform->Up();
	float3 rightMovement = translateVector.x * cameraTransform->Right();

	cameraTransform->Translate(upMovement + rightMovement);
}

void ModuleCamera::OrbitMouse()
{
	//It rotates around a point "x" at distance "y" in front direction, "y" value increases with zoom out, and decreases with zoom in
	if(zoomAmount > 1.0f)
	{
		float3 orbitCenter = cameraTransform->GetWorldPosition() + (cameraTransform->Forward() * zoomAmount);
		App->renderer->DrawCross(orbitCenter, zoomAmount);
		MovementMouseDrag(movementOrbitSpeed * App->time->GetEditorDeltaTime() * zoomAmount / ORBIT_FORCE_REDUCTION);
		cameraTransform->LookAt(orbitCenter);
	}
	else
	{
		RotationMouse();
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
		cameraTransform->Translate(cameraTransform->Forward() * movementTemp);
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
		cameraTransform->Translate(cameraTransform->Forward() * movementTemp);
	}
	else
	{
		zoomAmount = 0.0f;
	}
}

void ModuleCamera::RotationMouse()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) && !(App->input->GetKeyPressed(SDL_SCANCODE_LALT) || App->input->GetKeyPressed(SDL_SCANCODE_RALT)))
	{
		float2 mouseInput = App->input->GetMouseMotion();

		float2 degEulerRotation = mouseInput * rotationSpeed * App->time->GetEditorDeltaTime();
		degEulerRotation *= -1;

		cameraTransform->Rotate(Quat::RotateAxisAngle(float3::unitY, DegToRad(degEulerRotation.x)), Space::World);
		cameraTransform->Rotate(Quat::RotateAxisAngle(cameraTransform->Right(), DegToRad(degEulerRotation.y)));
	}
}

void ModuleCamera::MovementKeyBoard()
{
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT))
	{
		float3 input = GetWASDQEInput();
		float3 translateVector = input.x * -cameraTransform->Right() + input.y * cameraTransform->Up() + input.z * cameraTransform->Forward();
		cameraTransform->Translate(translateVector * movementSpeed * CalculateShiftDeltaMultiplier());
	}
}

void ModuleCamera::RotationKeyboard()
{
	float2 keyboardInput = GetArrowKeysInput();
	float2 degEulerRotation = KEYBOARD_ROTATION_SPEED_MULTIPLIER * keyboardInput * rotationSpeed * App->time->GetEditorDeltaTime();
	degEulerRotation *= -1;

	cameraTransform->Rotate(Quat::RotateAxisAngle(float3::unitY, DegToRad(degEulerRotation.x)), Space::World);
	cameraTransform->Rotate(Quat::RotateAxisAngle(cameraTransform->Right(), DegToRad(degEulerRotation.y)));
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

float ModuleCamera::CalculateShiftDeltaMultiplier() const
{
	float shiftDeltaMultiplier = App->time->GetEditorDeltaTime();

	if (App->input->GetKeyPressed(SDL_SCANCODE_LSHIFT) || App->input->GetKeyPressed(SDL_SCANCODE_RSHIFT))
	{
		shiftDeltaMultiplier *= SHIFT_MULTIPLIER;
	}

	return shiftDeltaMultiplier;
}

float3 ModuleCamera::GetWASDQEInput() const
{
	float3 input = float3::zero;
	
	if (App->input->GetKeyPressed(SDL_SCANCODE_D))
	{
		input.x += 1;
	}
	if (App->input->GetKeyPressed(SDL_SCANCODE_A))
	{
		input.x += -1;
	}
	
	if (App->input->GetKeyPressed(SDL_SCANCODE_E))
	{
		input.y += 1;
	}
	if (App->input->GetKeyPressed(SDL_SCANCODE_Q))
	{
		input.y += -1;
	}

	if (App->input->GetKeyPressed(SDL_SCANCODE_W))
	{
		input.z += 1;
	}
	if (App->input->GetKeyPressed(SDL_SCANCODE_S))
	{
		input.z += -1;
	}

	return input;
}

float2 ModuleCamera::GetArrowKeysInput() const
{
	float2 input = float2::zero;

	if (App->input->GetKeyPressed(SDL_SCANCODE_RIGHT))
	{
		input.x += 1;
	}
	if (App->input->GetKeyPressed(SDL_SCANCODE_LEFT))
	{
		input.x += -1;
	}

	if (App->input->GetKeyPressed(SDL_SCANCODE_UP))
	{
		input.y += 1;
	}
	if (App->input->GetKeyPressed(SDL_SCANCODE_DOWN))
	{
		input.y += -1;
	}

	return input;
}

void ModuleCamera::EnableCamera(Camera* camera)
{
	if(this->camera == camera) return;

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
