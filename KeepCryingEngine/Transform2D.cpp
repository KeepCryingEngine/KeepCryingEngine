#include "Transform2D.h"
#include "GameObject.h"


Transform2D::Transform2D() :
	Component(Transform2D::TYPE)
{
}


Transform2D::~Transform2D()
{
}

float3 Transform2D::GetWorldPosition() const
{
	return GetParentWorldPosition() + localPosition;
}

float3 Transform2D::GetLocalPosition() const
{
	return localPosition;
}

void Transform2D::SetLocalPosition(const float3 & position)
{
	if (!isLocked)
	{
		localPosition = position;
	}
}

void Transform2D::SetWorldPosition(const float3 & position)
{
	if (!isLocked)
	{
		localPosition = position - GetParentWorldPosition();
	}
}

float2 Transform2D::GetSize() const
{
	return size;
}

//Top Left Of the UI Element bounds
float3 Transform2D::GetMinPosition() const
{
	return GetLocalPosition() - float3(size.Mul(pivot),0);
}

//Bottom Right Of the UI Element bounds
float3 Transform2D::GetMaxPosition() const
{
	return GetMinPosition() + float3(size,0);
}

float3 Transform2D::GetParentWorldPosition() const
{
	float3 parentLocalPosition = float3::zero;
	GameObject *parent = gameObject->GetParent();
	if (parent != nullptr)
	{
		parentLocalPosition = gameObject->GetComponent<Transform2D>()->GetLocalPosition();
	}
	return parentLocalPosition;
}
