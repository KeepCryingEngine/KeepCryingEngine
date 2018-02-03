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
	~Transform();

	void DrawUI()override;
	float4x4 GetAcumulatedTransform();
public:
	float3 position;
	Quat rotation;
	float3 scale;
};

#endif // !_TRANSFORM_H_