#include "Image.h"

#include "Application.h"
#include "ModuleTexture.h"
#include "GameObject.h"
#include "Texture.h"
#include "json_serializer.h"

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

std::vector<Component::Type> Image::GetNeededComponents() const
{
	return { Component::Type::Transform2D };
}

void Image::DrawUI()
{
	if (ImGui::CollapsingHeader("Image"))
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
	{
		App->texture->Release(texture);
	}

	texture = App->texture->GetAsset(path);
	
	if (texture != nullptr)
	{
		texturePath = path;
	}
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

void Image::PreLoad(const nlohmann::json & json)
{
	enabled = json["enabled"];
	from_json(json["color"], color);
	SetTextureByPath(json["texturePath"].get<string>());
}

void Image::Load(const nlohmann::json & json)
{}

void Image::Save(nlohmann::json & json) const
{
	json["type"] = type;
	json["enabled"] = enabled;
	nlohmann::json jsonColor;
	to_json(jsonColor, color);
	json["color"] = jsonColor;

	json["texturePath"] = texturePath.string();
}