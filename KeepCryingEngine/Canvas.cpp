#include "Canvas.h"
#include "GameObject.h"

Canvas::Canvas():Component(Canvas::TYPE)
{}

Canvas::~Canvas()
{}

std::vector<Component::Type> Canvas::GetNeededComponents() const
{
	return { Component::Type::Transform2D };
}

void Canvas::DrawUI()
{
	if(ImGui::CollapsingHeader("Canvas"))
	{
		ImGui::PushID(gameObject->GetId());
		if(ImGui::Checkbox("Active", &enabled))
		{
			SetEnable(enabled);
		}
		ImGui::SameLine();
		ImGui::PopID();
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
	}
}