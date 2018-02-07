#ifndef _MODULECAMERA_H_
#define _MODULECAMERA_H_

#include <MathGeoLib.h>

#include "Module.h"

class Camera;

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	virtual ~ModuleCamera();

	bool Init() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;
	bool CleanUp() override;

	float GetMoveSpeed() const;
	float GetRotationSpeed() const;
	float GetDragSpeed() const;
	float GetOrbitSpeed() const;
	float GetZoomSpeed() const;

	void SetMoveSpeed(float speed);
	void SetRotationSpeed(float speed);
	void SetDragSpeed(float speed);
	void SetOrbitSpeed(float speed);
	void SetZoomSpeed(float speed);

	void EnableCamera(Camera* camera);

	// Camera* GetEnabledCamera() const;

private:
	void Rotation(float deltaTimeS);
	void Movement(float shiftDeltaMultiplier);

	void MovementMouse(float shiftDeltaMultiplier);
	void MovementMouseDrag(float shiftDeltaMultiplier);
	void MovementMouseZoom(float shiftDeltaMultiplier);
	void MovementWheelZoom(float shiftDeltaMultiplier);
	void RotateMouse(float deltaTimeS);
	void RotateMouseOrbit(float deltaTimeS);
	void RotateMouseRotation(float deltaTimeS);

	void MovementKeyBoard(float shiftDeltaMultiplier);
	void RotateKeyboard(float deltaTimeS);

public:
	Camera* camera = nullptr;

private:
	float movementSpeed = 10.0f;
	float rotationSpeed = 2.0f;
	float movementDragSpeed = 3.0f;
	float movementOrbitSpeed = 3.0f;
	float movementZoomSpeed = 3.0f;

	float zoomAmount = 10.0f;

	Camera* enabledCamera = nullptr;

	static const float SHIFT_MULTIPLIER;
	static const float WHEEL_FORCE;
	static const float ORBIT_FORCE_REDUCTION;
};

#endif // !_MODULECAMERA_H_