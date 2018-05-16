#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_

#include <float3.h>

#include "Module.h"

class ModuleLight : public Module
{
public:
	ModuleLight();
	virtual ~ModuleLight();

	update_status Update() override;

	const float3& GetPosition() const;
	void SetPosition(const float3& position);

	const float3& GetDirection() const;
	void SetDirection(const float3& direction);

	void SetRotation(float amount);

private:
	void DrawLight() const;

private:
	const float3 INITIAL_POSITION = float3{ -100.0f, 0.0f, 0.0f };
	const float3 INITIAL_ROTATION = float3{ -1.0f, 1.0f, 0.0f };

	float3 basePosition = INITIAL_POSITION;
	float3 currentPosition = INITIAL_POSITION;

	float3 baseDirection = INITIAL_ROTATION;
	float3 currentDirection = INITIAL_ROTATION;
};

#endif