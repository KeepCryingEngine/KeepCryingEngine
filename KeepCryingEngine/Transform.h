#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"

#include <float3.h>
#include <Quat.h>

class Transform : Component
{
public:
	Transform();
	~Transform();

private:
	float3 position;
	Quat rotation;
	float3 scale;
};

#endif // !_TRANSFORM_H_