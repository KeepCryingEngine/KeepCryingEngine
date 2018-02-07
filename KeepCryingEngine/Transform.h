#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"

#include <float3.h>
#include <float4x4.h>
#include <Quat.h>

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	void DrawUI() override;
	float4x4 GetAcumulatedTransform();
	
	float3x3 GetAcumulatedRotation();
	float4x4 GetAcumulatedTransformWithoutScale();

	virtual std::vector<ComponentType> GetProhibitedComponents() const override;

public:
	float3 position;
	Quat rotation;
	float3 scale;
};

#endif // !_TRANSFORM_H_