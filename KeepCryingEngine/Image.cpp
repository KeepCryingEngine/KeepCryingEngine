#include "Image.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "GameObject.h"
#include "Texture.h"

using namespace std;

Image::Image(): Component(Image::TYPE)
{}

Image::~Image()
{}

void Image::Awake()
{
	isHovereableUI = true;
	gameObject->SetHovereableUI(true);
}

void Image::Destroy()
{
	isHovereableUI = false;
	gameObject->CheckIfHovereableUI();
}

void Image::SetEnable(bool value)
{
	Component::SetEnable(value);
	if(value)
	{
		isHovereableUI = true;
		gameObject->SetHovereableUI(true);
	}
	else
	{
		isHovereableUI = false;
		gameObject->CheckIfHovereableUI();
	}
}

void Image::DrawUI()
{
	if (ImGui::CollapsingHeader("Image"))
	{
		ImGui::PushID(gameObject->GetId());
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		ImGui::PopID();
		SetEnable(enabled);
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
		static char texturePath[252] = "Lenna.png";
		ImGui::InputText("##setTexture", texturePath, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			std::experimental::filesystem::path path = "Assets/";
			path /= texturePath;

			SetTextureByPath(path);
		}	
	}
}

void Image::SetTextureByPath(const std::experimental::filesystem::path & path)
{
	if (texture != nullptr)
		App->texture->Release(texture);

	texture = App->texture->GetAsset(path);
}

void Image::SetTexture(Texture & texture)
{
	this->texture = &texture;
}

void Image::SetColor(float4 color)
{
	this->color = color;
}

float4 Image::GetColor() const
{
	return color;
}

Texture * Image::GetTexture() const
{
	return texture;
}
