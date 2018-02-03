#include "Transform.h"



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