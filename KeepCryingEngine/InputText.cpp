#include "InputText.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "Text.h"
#include "ModuleFont.h"
#include "Transform2D.h"
#include "Image.h"

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

void InputText::RealUpdate()
{
	cursorGameObject->SetEnabled(onFocus);
	ManagePlaceHolder();

	if(onFocus)
	{
		currentTextUnderPassword = App->input->GetCurrentText();
		textGameObject->GetComponent<Text>()->SetText(currentTextUnderPassword);

		ManageCursor();
	}
	else
	{		
		dirtyText = true;
	}

	if(passwordMode)
	{
		std::string passwordMessage;
		for(size_t i = 0; i < currentTextUnderPassword.length(); i++)
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
		
		if(ImGui::Checkbox("Password", &passwordMode))
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

void InputText::SetCursorGameObject(GameObject & g)
{
	cursorGameObject = &g;
	g.GetComponent<Image>()->SetTextureByPath("Assets/Cursor.png");
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

void InputText::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);
	passwordMode = json["passwordMode"];
}

void InputText::Load(const nlohmann::json & json)
{
	placeHolderGameObject = App->scene->Get(json["placeHolderGameObjectID"]);
	textGameObject = App->scene->Get(json["textGameObjectID"]);
	cursorGameObject = App->scene->Get(json["cursorGameObjectID"]);
}

void InputText::Save(nlohmann::json & json) const
{
	Component::Save(json);
	json["passwordMode"] = passwordMode;
	json["placeHolderGameObjectID"] = placeHolderGameObject->GetId();
	json["textGameObjectID"] = textGameObject->GetId();
	json["cursorGameObjectID"] = cursorGameObject->GetId();
}

void InputText::ManageCursor()
{
	if(App->input->IsShifting())
	{
		ShiftCursor();
	}
	else
	{
		NormalCursor();
	}
	
}

void InputText::ShiftCursor()
{
	int cursor0 = App->input->GetActualTextPos();
	int cursor1 = App->input->GetShiftInitialTextPos();
	int leftCursor = min(cursor0,cursor1);
	int rightCursor = max(cursor0,cursor1);

	Text* tempText = textGameObject->GetComponent<Text>();
	int sizeX = 0;
	int sizeY = 0;
	App->font->GetSizeFromString(tempText->GetFont(), tempText->GetText().substr(leftCursor, rightCursor-leftCursor), sizeX, sizeY);
	int width = 0;
	int height = 0;
	App->font->GetSizeFromString(tempText->GetFont(), tempText->GetText().substr(0, leftCursor), width, height);

	float3 pos = textGameObject->GetComponent<Transform2D>()->GetWorldPosition();
	pos.x -= tempText->GetTextureSize().x / 2.0f;
	pos.x += width;
	pos.x += sizeX / 2.0f;

	Transform2D* cursorTransform = cursorGameObject->GetComponent<Transform2D>();
	cursorTransform->SetWorldPosition(pos);
	float textureHeight = tempText->GetTextureSize().y * 1.25f;
	cursorTransform->SetSize(float2((float)sizeX, textureHeight));

}

void InputText::NormalCursor()
{
	int cursor = App->input->GetActualTextPos();
	Text* tempText = textGameObject->GetComponent<Text>();
	int width = 0;
	int height = 0;
	App->font->GetSizeFromString(tempText->GetFont(), tempText->GetText().substr(0, cursor), width, height);
	float3 pos = textGameObject->GetComponent<Transform2D>()->GetWorldPosition();
	pos.x -= tempText->GetTextureSize().x / 2.0f;
	pos.x += width;
	Transform2D* cursorTransform = cursorGameObject->GetComponent<Transform2D>();
	cursorTransform->SetWorldPosition(pos);
	float h = tempText->GetTextureSize().y * 1.25f;
	cursorTransform->SetSize(float2(h*0.1f, h));
}

void InputText::ManagePlaceHolder()
{
	if(textGameObject->GetComponent<Text>()->GetText() == "" && !onFocus)
	{
		placeHolderGameObject->GetComponent<Text>()->enabled = true;
	}
	else
	{
		placeHolderGameObject->GetComponent<Text>()->enabled = false;
	}
}
