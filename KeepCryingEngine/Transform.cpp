#include "Transform.h"



Transform::Transform() :Component(ComponentType::Transform), position(), rotation(), scale(1,1,1)
{
}


Transform::~Transform()
{
}


void Transform::DrawUI()
{
	ImGui::Begin("Transform");
	ImGui::DragFloat3("Position:",position.ptr());
	ImGui::DragFloat3("Rotation:",rotation.ToEulerXYZ().ptr());
	ImGui::DragFloat3("Scale:",scale.ptr());
	ImGui::End();
}