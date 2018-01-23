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
	bool CleanUp() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

	float4x4 GetViewMatrix()const;
	float4x4 GetProyectionMatrix()const;

private:
	Frustum frustum;
};

#endif // !_MODULECAMERA_H_

