#include "Button.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleGameUI.h"
#include "ModuleInput.h"
#include "Image.h"
#include "GameObject.h"
#include <SDL.h>
#include "json_serializer.h"

Button::Button(): Component(Button::TYPE)
{
	for(int i = 0; i < (int)ButtonState::NUMBER_STATES; i++)
	{
		colors[i] = float4().one;
		textures[i] = nullptr;
		texturesPath[i] = "";
	}
}

Button::~Button()
{}

void Button::Awake()
{
	isFocuseableUI = true;
	isHovereableUI = true;
	gameObject->SetFocuseableUI(true);
	gameObject->SetHovereableUI(true);
}

void Button::Destroy()
{
	isFocuseableUI = false;
	isHovereableUI = false;
	gameObject->CheckIfFocuseableUI();
	gameObject->CheckIfHovereableUI();
}

std::vector<Component::Type> Button::GetNeededComponents() const
{
	return { Component::Type::Transform2D };
}

void Button::SetEnable(bool value)
{
	Component::SetEnable(value);
	Image* img = gameObject->GetComponent<Image>();
	if(value)
	{
		isFocuseableUI = true;
		isHovereableUI = true;
		gameObject->SetFocuseableUI(true);
		gameObject->SetHovereableUI(true);
		state = ButtonState::NORMAL;
	}
	else
	{
		isFocuseableUI = false;
		isHovereableUI = false;
		gameObject->CheckIfFocuseableUI();
		gameObject->CheckIfHovereableUI();
		state = ButtonState::DISABLED;
	}
	switch(transitionType)
	{
		case Transition::COLOR:
			img->SetTexture(*textures[(unsigned int)ButtonState::NORMAL]);
			img->SetColor(colors[(unsigned int)state]);
			break;
		case Transition::SPRITE:
			img->SetTexture(*textures[(unsigned int)state]);
			break;
		default:
			assert(false);
			break;
	}
}

void Button::RealUpdate()
{
	Image* img = gameObject->GetComponent<Image>();
	if(img != nullptr)
	{
		switch(transitionType)
		{
			case Transition::COLOR:
			{
				img->SetTexture(*textures[(unsigned int)ButtonState::NORMAL]);
				img->SetColor(colors[(unsigned int)state]);
			}
				break;
			case Transition::SPRITE:
			{
				img->SetTexture(*textures[(unsigned int)state]);
			}
				break;
			default:
				assert(false);
				break;
		}
	}

	state = ButtonState::NORMAL;
}

void Button::DrawUI()
{
	if(ImGui::CollapsingHeader("Button"))
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
		static char textureNormalPath[252] = "Lenna.png";
		ImGui::Text("Normal"); ImGui::SameLine();
		ImGui::InputText("##buttonNormalTexture", textureNormalPath, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			std::experimental::filesystem::path path = "Assets/";
			path /= textureNormalPath;
			texturesPath[(unsigned int)ButtonState::DISABLED] = path;

			SetTextureByPath(path, ButtonState::NORMAL);
		}

		static int transitionMode = (int)transitionType;
		if(ImGui::Combo("Transition", &transitionMode, "Color\0Sprite"))
		{
			transitionType = (Transition)transitionMode;
		}

		switch(transitionType)
		{
			case Transition::COLOR:
			{
				ImGui::DragFloat4("Normal", colors[(unsigned int)ButtonState::NORMAL].ptr(), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat4("Hover", colors[(unsigned int)ButtonState::HOVER].ptr(), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat4("Pressed", colors[(unsigned int)ButtonState::PRESSED].ptr(), 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat4("Disabled", colors[(unsigned int)ButtonState::DISABLED].ptr(), 0.01f, 0.0f, 1.0f);
			}
			break;
			case Transition::SPRITE:
			{
				ImGui::PushID(gameObject->GetId());
				static char textureHoverPath[252] = "rock.jpg";
				ImGui::Text("Hover"); ImGui::SameLine();
				ImGui::InputText("##buttonHoverTexture", textureHoverPath, 252); ImGui::SameLine();
				if(ImGui::Button("Set Texture"))//TODO:CHANGE NAME/ID
				{
					std::experimental::filesystem::path path = "Assets/";
					path /= textureHoverPath;
					texturesPath[(unsigned int)ButtonState::HOVER] = path;

					SetTextureByPath(path, ButtonState::HOVER);
				}
				ImGui::PopID();

				ImGui::PushID(gameObject->GetId() + 1);
				static char texturePressedPath[252] = "exodia.dds";
				ImGui::Text("Pressed"); ImGui::SameLine();
				ImGui::InputText("##buttonPressedTexture", texturePressedPath, 252); ImGui::SameLine();
				if(ImGui::Button("Set Texture"))//TODO:CHANGE NAME/ID
				{
					std::experimental::filesystem::path path = "Assets/";
					path /= texturePressedPath;
					texturesPath[(unsigned int)ButtonState::PRESSED] = path;

					SetTextureByPath(path, ButtonState::PRESSED);
				}
				ImGui::PopID();

				ImGui::PushID(gameObject->GetId() + 2);
				static char textureDisabledPath[252] = "Baker_house.png";
				ImGui::Text("Disabled"); ImGui::SameLine();
				ImGui::InputText("##buttonDisabledTexture", textureDisabledPath, 252); ImGui::SameLine();
				if(ImGui::Button("Set Texture"))//TODO:CHANGE NAME/ID
				{
					std::experimental::filesystem::path path = "Assets/";
					path /= textureDisabledPath;
					texturesPath[(unsigned int)ButtonState::DISABLED] = path;

					SetTextureByPath(path, ButtonState::DISABLED);
				}
				ImGui::PopID();
			}
			break;
			default:
				assert(false);
				break;
		}
	}
}

void Button::OnHovering()
{
	state = ButtonState::HOVER;
}

void Button::OnClick()
{
	state = ButtonState::PRESSED;
}

void Button::SetTextureByPath(const std::experimental::filesystem::path & path, ButtonState state)
{
	if(textures[(unsigned int)state] != nullptr)
	{
		App->texture->Release(textures[(unsigned int)state]);
	}

	textures[(unsigned int)state] = App->texture->GetAsset(path);
}

void Button::SetColor(float4 newColor, ButtonState state)
{
	colors[(unsigned int)state] = newColor;
}

Texture * Button::GetTexture(ButtonState state) const
{
	return textures[(unsigned int) state];
}

float4 Button::GetColor(ButtonState state) const
{
	return colors[(unsigned int)state];
}

void Button::Load(const nlohmann::json & json)
{
	state = json["state"];
	transitionType = json["transitionType"];
	nlohmann::json jsonColors = json["colors"];
	nlohmann::json jsonPaths = json["paths"];
	for(int i = 0; i < (unsigned int)ButtonState::NUMBER_STATES; i++)
	{
		from_json(jsonColors[i], colors[i]);
		SetTextureByPath(jsonPaths[i].get<string>(),(ButtonState)i);
	}

}

void Button::Save(nlohmann::json & json) const
{
	/*

	Relevant information:

	type
	state
	transitionType
	colors[(unsigned int)ButtonState::NUMBER_STATES];
	texturesPath[(unsigned int)ButtonState::NUMBER_STATES];
	*/

	json["type"] = type;
	json["state"] = state;
	json["transitionType"] = transitionType;
	nlohmann::json jsonColors;
	nlohmann::json jsonPaths;
	for(int i = 0; i < (unsigned int)ButtonState::NUMBER_STATES; i++)
	{
		nlohmann::json jsonColor;
		to_json(jsonColor, colors[i]);
		jsonColors.push_back(jsonColor);

		jsonPaths.push_back(texturesPath[i].string());
	}
	json["colors"] = jsonColors;
	json["paths"] = jsonPaths;
	
}