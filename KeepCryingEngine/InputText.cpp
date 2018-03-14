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

void InputText::Start()
{
	currentTextUnderPassword = textGameObject->GetComponent<Text>()->GetText();
}

void InputText::Destroy()
{
	isFocuseableUI = false;
	isHovereableUI = false;
	gameObject->CheckIfFocuseableUI();
	gameObject->CheckIfHovereableUI();
}

std::vector<Component::Type> InputText::GetNeededComponents() const
{
	return { Component::Type::Transform2D };
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
		currentTextUnderPassword = App->input->GetCurrentText();
		textGameObject->GetComponent<Text>()->SetText(currentTextUnderPassword);
	}
	else
	{
		dirtyText = true;
	}

	if(passwordMode)
	{
		std::string passwordMessage;
		for(int i = 0; i < currentTextUnderPassword.length(); i++)
		{
			passwordMessage.push_back('*');
		}
		textGameObject->GetComponent<Text>()->SetText(passwordMessage);
	}
	else
	{
		if(dirtyText)
		{
			textGameObject->GetComponent<Text>()->SetText(currentTextUnderPassword);
		}
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
		
		if(ImGui::Checkbox("Active", &passwordMode))
		{
			SetPasswordMode(passwordMode);
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

void InputText::SetPasswordMode(bool value)
{
	passwordMode = value;
}

bool InputText::GetPasswordMode() const
{
	return passwordMode;
}

GameObject * InputText::GetPlaceHolderGameObject() const
{
	return placeHolderGameObject;
}

GameObject * InputText::GetTextGameObject() const
{
	return textGameObject;
}

void InputText::Load(const nlohmann::json & json)
{

}

void InputText::Save(nlohmann::json & json) const
{

}