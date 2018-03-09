#include "InputText.h"

#include "Application.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Text.h"

InputText::InputText():Component(InputText::TYPE)
{}

InputText::~InputText()
{}

void InputText::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(onFocus)
	{
		textGameObject->GetComponent<Text>()->SetText(App->input->GetCurrentText());
	}
	if(textGameObject->GetComponent<Text>()->GetText() == "")
	{
		placeHolderGameObject->GetComponent<Text>()->enabled = true;
	}
	else
	{
		placeHolderGameObject->GetComponent<Text>()->enabled = false;
	}
}

void InputText::DrawUI()
{
	if(ImGui::CollapsingHeader("InputText"))
	{
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
	}
}

void InputText::OnFocus()
{
	onFocus = true;
	App->input->SetStartToRead(true);
	App->input->SetText(textGameObject->GetComponent<Text>()->GetText().c_str());
}

void InputText::SetPlaceHolderGameObject(GameObject & g)
{
	placeHolderGameObject = &g;
}

void InputText::SetTextGameObject(GameObject & g)
{
	textGameObject = &g;
}

GameObject * InputText::GetPlaceHolderGameObject() const
{
	return placeHolderGameObject;
}

GameObject * InputText::GetTextGameObject() const
{
	return textGameObject;
}
