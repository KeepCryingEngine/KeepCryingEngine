#ifndef _MODULECAMERA_H_
#define _MODULECAMERA_H_

#include <MathGeoLib.h>

#include "Module.h"

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();

	bool Init() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

	void SetFOV(float radians);
	void SetAspectRatio();
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetPosition(float3 position);
	void SetMoveSpeed(float speed);
	void SetRotationSpeed(float speed);
	void SetDragSpeed(float speed);
	void SetOrbitSpeed(float speed);
	void SetZoomSpeed(float speed);

	float GetMoveSpeed() const;
	float GetRotationSpeed() const;
	float GetDragSpeed() const;
	float GetOrbitSpeed() const;
	float GetZoomSpeed() const;
	float GetFOV() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float3 GetPosition() const;
	float4x4 GetViewMatrix() const;
	float4x4 GetProyectionMatrix() const;

	float3 GetUpVector() const;
	float3 GetSideVector() const;
	float3 GetFrontVector() const;
	void LookAt(float3 point);

private:
	void SetUpFrustum();
	float ComputeHorizontalFov(float radians, float width, float height) const;

	void Rotation(float deltaTimeS);
	void Movement(float shiftDeltaMultiplier);

	void MovementMouse(float shiftDeltaMultiplier);
	void MovementMouseDrag(float shiftDeltaMultiplier);
	void MovementMouseZoom(float shiftDeltaMultiplier);
	void RotateMouse(float deltaTimeS);
	void RotateMouseOrbit(float deltaTimeS);
	void RotateMouseRotation(float deltaTimeS);

	void MovementKeyBoard(float shiftDeltaMultiplier);
	void RotateKeyboard(float deltaTimeS);

private:
	Frustum frustum;

	float movementSpeed = 10.0f;
	float rotationSpeed = 2.0f;
	float movementDragSpeed = 3.0f;
	float movementOrbitSpeed = 3.0f;
	float movementZoomSpeed = 3.0f;

	float zoomAmount = 10.0f;

	static const float SHIFT_MULTIPLIER;
};

#endif // !_MODULECAMERA_H_