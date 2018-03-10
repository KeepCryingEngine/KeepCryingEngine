#include "Canvas.h"
#include "GameObject.h"

Canvas::Canvas():Component(Canvas::TYPE)
{}

Canvas::~Canvas()
{}

void Canvas::DrawUI()
{
	if(ImGui::CollapsingHeader("Canvas"))
	{
		ImGui::PushID(gameObject->GetId());
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		ImGui::PopID();
		SetEnable(enabled);
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
	}
}
