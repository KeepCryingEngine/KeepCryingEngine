#ifndef _MODULECAMERA_H_
#define _MODULECAMERA_H_

#include <MathGeoLib.h>
#include <vector>

#include "Module.h"


class Camera;
class GameObject;
class Transform;

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	virtual ~ModuleCamera();

	bool Init() override;
	update_status Update() override;
	bool CleanUp() override;

	void Play() override;
	void Stop() override;

	void Subscribe(Camera* c);
	void UnSubscribe(Camera* c);

	std::vector<Camera*> GetAllCameras() const;

	void EnableCamera(Camera* camera);
	Camera* GetEnabledCamera() const;
	Camera* GetPlayOrEditorCamera() const;
	const LineSegment & GetLastRay()const;
	
	float GetMoveSpeed() const;
	void SetMoveSpeed(float speed);

	float GetRotationSpeed() const;
	void SetRotationSpeed(float speed);

	float GetDragSpeed() const;
	void SetDragSpeed(float speed);

	float GetOrbitSpeed() const;
	void SetOrbitSpeed(float speed);

	float GetZoomSpeed() const;
	void SetZoomSpeed(float speed);

private:
	void Rotation();
	void Movement();

	void MovementMouse();
	void MovementMouseDrag(float shiftDeltaMultiplier);
	void MovementMouseZoom(float shiftDeltaMultiplier);
	void MovementWheelZoom(float shiftDeltaMultiplier);
	void Orbit();
	void OrbitMouse();
	void RotationMouse();

	void MovementKeyBoard();
	void RotationKeyboard();

	void ScenePick();

	float CalculateShiftDeltaMultiplier() const;
	float3 GetWASDQEInput() const;
	float2 GetArrowKeysInput() const;

public:
	GameObject* cameraGameObject = nullptr;
	Transform* cameraTransform = nullptr;
	Camera * camera = nullptr;

private:
	float movementSpeed = 10.0f;
	float rotationSpeed = 5.0f;
	float movementDragSpeed = 3.0f;
	float movementOrbitSpeed = 3.0f;
	float movementZoomSpeed = 3.0f;

	float zoomAmount = 10.0f;

	LineSegment lastRay;

	Camera* enabledCamera = nullptr;
	Camera** modeCamera = nullptr;

	std::vector<Camera*> allCameras;

	static const float SHIFT_MULTIPLIER;
	static const float WHEEL_FORCE;
	static const float ORBIT_FORCE_REDUCTION;
	static const float KEYBOARD_ROTATION_SPEED_MULTIPLIER;
};

#endif // !_MODULECAMERA_H_