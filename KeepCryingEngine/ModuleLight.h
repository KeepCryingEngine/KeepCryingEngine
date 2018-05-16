#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_

#include <float3.h>

#include "Module.h"

class ModuleLight : public Module
{
public:
	ModuleLight();
	virtual ~ModuleLight();

	const float3& GetDirection() const;
	void SetDirection(const float3& direction);

	void SetRotation(float amount);

private:
	const float3 INITIAL_ROTATION = float3{ -1.0f, 1.0f, 0.0f };

	float3 baseDirection = INITIAL_ROTATION;
	float3 currentDirection = INITIAL_ROTATION;
};

#endif