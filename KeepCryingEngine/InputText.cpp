#include "InputText.h"

#include "Application.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Text.h"

InputText::InputText():Component(InputText::TYPE)
{}

InputText::~InputText()
{}

void InputText::Awake()
{
	isFocuseableUI = true;
	isHovereableUI = true;
	gameObject->SetFocuseableUI(true);
	gameObject->SetHovereableUI(true);
}

void InputText::Destroy()
{
	isFocuseableUI = false;
	isHovereableUI = false;
	gameObject->CheckIfFocuseableUI();
	gameObject->CheckIfHovereableUI();
}

void InputText::SetEnable(bool value)
{
	Component::SetEnable(value);
	if(value)
	{
		isFocuseableUI = true;
		isHovereableUI = true;
		gameObject->SetFocuseableUI(true);
		gameObject->SetHovereableUI(true);
	}
	else
	{
		isFocuseableUI = false;
		isHovereableUI = false;
		gameObject->CheckIfFocuseableUI();
		gameObject->CheckIfHovereableUI();
	}
}

void InputText::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(onFocus)
	{
		textGameObject->GetComponent<Text>()->SetText(App->input->GetCurrentText());
	}
	else
	{
		dirtyText = true;
	}

	if(textGameObject->GetComponent<Text>()->GetText() == "")
	{
		placeHolderGameObject->GetComponent<Text>()->enabled = true;
	}
	else
	{
		placeHolderGameObject->GetComponent<Text>()->enabled = false;
	}
	onFocus = false;
}

void InputText::DrawUI()
{
	if(ImGui::CollapsingHeader("InputText"))
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

void InputText::OnFocus()
{
	onFocus = true;
	App->input->SetStartToRead(true);
	if(dirtyText)
	{
		App->input->SetText(textGameObject->GetComponent<Text>()->GetText().c_str());
		dirtyText = false;
	}
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
