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
	float3 Orientation();
	void LookAt(float3 point);

	float4x4 GetViewMatrix()const;
	float4x4 GetProyectionMatrix()const;

private:
	Frustum frustum;
	float movementSpeed=10.0f;
	float rotationSpeed = 10.0f;
};

#endif // !_MODULECAMERA_H_

