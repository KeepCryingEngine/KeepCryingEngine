#include "Transform.h"



Transform::Transform() :
	Component(ComponentType::Transform),
	position(),
	rotation(),
	scale(1,1,1)
{
}


Transform::~Transform()
{
}
