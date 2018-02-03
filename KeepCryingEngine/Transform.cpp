#include "Transform.h"

#include "GameObject.h"

Transform::Transform() :Component(ComponentType::Transform), position(float3::zero), rotation(Quat::identity), scale(float3::one)
{
}


Transform::~Transform()
{
}


void Transform::DrawUI()
{
	if(ImGui::CollapsingHeader("Transform"))
	{
		ImGui::DragFloat3(" Position", position.ptr());
		ImGui::DragFloat3(" Rotation", rotation.ToEulerXYZ().ptr());
		ImGui::DragFloat3(" Scale", scale.ptr());
	}
}

float4x4 Transform::GetAcumulatedTransform()
{
	if (gameObject->GetParent()) {
		Transform* parent = (Transform*)gameObject->GetParent()->GetComponent(ComponentType::Transform);
		float4x4 parentTransformMatrix = parent->GetAcumulatedTransform();
		return parentTransformMatrix * float4x4::FromTRS(position, rotation.ToFloat4x4(), scale);
	}
	else 
	{
		return float4x4::identity;
	}
}
