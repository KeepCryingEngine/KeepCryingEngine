#include "Transform.h"

#include <MathGeoLib.h>

#include "GameObject.h"

using namespace std;

Transform::Transform() :Component(ComponentType::Transform), localPosition(float3::zero), localRotation(Quat::identity), localScale(float3::one)
{ }

Transform::~Transform()
{ }

void Transform::DrawUI()
{
	if(ImGui::CollapsingHeader("Transform"))
	{
		ImGui::DragFloat3(" Position", localPosition.ptr(),0.1f);
		float3 angles = RadToDeg(localRotation.ToEulerXYZ());
		if(ImGui::DragFloat3(" Rotation", angles.ptr()), 0.1f)
		{
			angles = DegToRad(angles);
			localRotation = Quat::FromEulerXYZ(angles.x, angles.y, angles.z);
		}
		ImGui::DragFloat3(" Scale", localScale.ptr(), 0.1f);
	}
}

vector<ComponentType> Transform::GetProhibitedComponents() const
{
	return { ComponentType::Transform };
}

const float3 & Transform::GetLocalPosition() const
{
	return localPosition;
}

const Quat & Transform::GetLocalRotation() const
{
	return localRotation;
}

const float3 & Transform::GetLocalScale() const
{
	return localScale;
}

void Transform::SetLocalPosition(const float3 & position)
{
	dirty = true;
	localPosition = position;
}

void Transform::SetLocalRotation(const Quat & rotation)
{
	dirty = true;
	localRotation = rotation;
}

void Transform::SetLocalScale(const float3 & scale)
{
	dirty = true;
	localScale = scale;
}

const float3 & Transform::GetWorldPosition() const
{
	RecalculateIfNecessary();
	return worldPosition;
}

const Quat & Transform::GetWorldRotation() const
{
	RecalculateIfNecessary();
	return worldRotation;
}

const float3 & Transform::GetWorldScale() const
{
	RecalculateIfNecessary();
	return worldScale;
}

/*
	Set local variables instead of world ones
	Then we calculate the world variables from the local ones when they are asked for
*/
void Transform::SetWorldPosition(const float3 & position)
{
	dirty = true;
	float3 worldPositionDelta = position - worldPosition;
	localPosition = localPosition + worldPositionDelta;
}

void Transform::SetWorldRotation(const Quat & rotation)
{
	dirty = true;
	Quat worldRotationDelta = rotation.Mul(worldRotation.Inverted());
	worldRotation = localRotation.Mul(worldRotationDelta);
}

void Transform::SetWorldScale(const float3 & scale)
{
	dirty = true;
	float3 worldScaleDelta = scale - worldScale;
	localScale = localScale + worldScaleDelta;
}

const float4x4 & Transform::GetModelMatrix() const
{
	RecalculateIfNecessary();
	return modelMatrix;
}

float4x4 Transform::GetLocalMatrix() const
{
	return float4x4::FromTRS(localPosition, localRotation.ToFloat4x4(), localScale);
}

float4x4 Transform::GetParentMatrix() const
{
	float4x4 parentMatrix = float4x4::identity;
	GameObject* parent = gameObject->GetParent();
	if (parent)
	{
		Transform* parentTransform = (Transform*)parent->GetComponent(ComponentType::Transform);
		parentMatrix = parentTransform->GetModelMatrix();
	}
	return parentMatrix;
}

/*
	Const function that changes mutable data
	It needs to be const because it must update the matrix only when necessary
*/
void Transform::RecalculateIfNecessary() const
{
	if (dirty) 
	{
		float4x4 localMatrix = GetLocalMatrix();
		float4x4 parentMatrix = GetParentMatrix();
		modelMatrix = parentMatrix * localMatrix;
		modelMatrix.Decompose(worldPosition, worldRotation, worldScale);
		dirty = false;
	}
}
