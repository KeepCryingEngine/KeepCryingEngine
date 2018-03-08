#include "Transform2D.h"
#include "GameObject.h"


Transform2D::Transform2D() :
	Component(Transform2D::TYPE),
	localPosition(0,0,0),
	size(100,100),
	anchor(0.5f, 0.5f),
	pivot(0.5f, 0.5f)
{
}


Transform2D::~Transform2D()
{
}

void Transform2D::DrawUI()
{
	if (ImGui::CollapsingHeader("Transform2D"))
	{
		ImGui::DragFloat3(" Position", localPosition.ptr(), 0.1f);
		ImGui::DragFloat2(" Size", size.ptr(), 0.1f);
		ImGui::DragFloat2(" Pivot", pivot.ptr(), 0.1f);
	}
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
