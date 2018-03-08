#include "Button.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleGameUI.h"
#include "ModuleInput.h"
#include <SDL.h>

Button::Button(): Component(Button::TYPE)
{}

Button::~Button()
{}

void Button::Awake()
{
	for(int i = 0; i < (int)ButtonState::NUMBER_STATES; i++)
	{
		colors[i] = float4().zero;
		textures[i] = nullptr;
	}
}

void Button::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(App->uiGame->GetFocusGameObject() == gameObject)
	{
		if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
		{
			state = ButtonState::PRESSED;
		}
		else
		{
			state = ButtonState::HOVER;
		}
	}
	else
	{
		state = ButtonState::DISABLED;
	}
}

void Button::DrawUI()
{
	if(ImGui::CollapsingHeader("Button"))
	{

		static char textureDisabledPath[252] = "Lenna.png";
		ImGui::Text("Disabled"); ImGui::SameLine();
		ImGui::InputText("##buttonDisabledTexture", textureDisabledPath, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			std::experimental::filesystem::path path = "Assets/";
			path /= textureDisabledPath;

			SetTextureByPath(path,ButtonState::DISABLED);
		}

		static int transitionMode = (int)transitionType;
		if(ImGui::Combo("Transition", &transitionMode, "Color\0Sprite")){
			transitionType = (Transition)transitionMode;
		}

		switch(transitionType)
		{
			case Transition::COLOR:
			{
				ImGui::DragFloat4("Disabled", colors[(unsigned int)ButtonState::DISABLED].ptr(),1.0f,0.0f,255.0f);
				ImGui::DragFloat4("Hover",colors[(unsigned int)ButtonState::HOVER].ptr(), 1.0f, 0.0f, 255.0f);
				ImGui::DragFloat4("Pressed", colors[(unsigned int)ButtonState::PRESSED].ptr(), 1.0f, 0.0f, 255.0f);
			}
				break;
			case Transition::SPRITE:
			{
				static char textureHoverPath[252] = "Lenna.png";
				ImGui::Text("Hover"); ImGui::SameLine();
				ImGui::InputText("##buttonHoverTexture", textureHoverPath, 252); ImGui::SameLine();
				if(ImGui::Button("Set Texture"))
				{
					std::experimental::filesystem::path path = "Assets/";
					path /= textureHoverPath;

					SetTextureByPath(path, ButtonState::HOVER);
				}

				static char texturePressedPath[252] = "Lenna.png";
				ImGui::Text("Pressed"); ImGui::SameLine();
				ImGui::InputText("##buttonPressedTexture", texturePressedPath, 252); ImGui::SameLine();
				if(ImGui::Button("Set Texture"))
				{
					std::experimental::filesystem::path path = "Assets/";
					path /= texturePressedPath;

					SetTextureByPath(path, ButtonState::PRESSED);
				}
			}
				break;
			default:
				assert(false);
				break;
		}

	}
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