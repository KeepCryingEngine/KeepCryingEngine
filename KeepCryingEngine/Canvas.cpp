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
		static bool uiEnable = enabled;
		ImGui::Checkbox("Active", &uiEnable); ImGui::SameLine();
		SetEnable(uiEnable);
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
	}
}
