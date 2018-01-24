#ifndef _MODULECAMERA_H_
#define _MODULECAMERA_H_

#include "Module.h"
#include <MathGeoLib.h>

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
	float GetFOV() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float3 GetPosition() const;
	float3 Orientation() const;
	void LookAt(float3 point);

	float4x4 GetViewMatrix() const;
	float4x4 GetProyectionMatrix() const;

private:
	void SetUpFrustum();
	float ComputeHorizontalFov(float radians, float width, float height) const;

	void MovementMouse(float shiftDeltaMultiplier);
	void MovementMouseDrag(float shiftDeltaMultiplier);
	void MovementMouseOrbit(float shiftDeltaMultiplier);
	void MovementMouseZoom(float shiftDeltaMultiplier);

	void MovementKeyBoard(float shiftDeltaMultiplier);

private:
	Frustum frustum;

	float movementSpeed = 10.0f;
	float rotationSpeed = 2.0f;
	float movementDragSpeed = 3.0f;
	float movementObritSpeed = 3.0f;
	float movementZoomSpeed = 3.0f;

	static const float SHIFT_MULTIPLIER;
};

#endif // !_MODULECAMERA_H_

