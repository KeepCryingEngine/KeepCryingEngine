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

	void SetFOV(float radians);
	void SetAspectRatio(float aspect);
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetPosition(float3 position);
	void SetOrientation(float3 rotation);
	void LookAt(float3 point);

	float4x4 GetViewMatrix()const;
	float4x4 GetProyectionMatrix()const;

private:
	Frustum frustum;
};

#endif // !_MODULECAMERA_H_

