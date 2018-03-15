#include "Text.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleFont.h"
#include "json_serializer.h"

Text::Text(): Component(Text::TYPE)
{}

Text::~Text()
{}

void Text::Start()
{
	font = App->font->LoadFont(currentFontPath, size);
	SetTexture(actualText);
}

std::vector<Component::Type> Text::GetNeededComponents() const
{
	return { Component::Type::Transform2D };
}

void Text::DrawUI()
{
	if(ImGui::CollapsingHeader("Text"))
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
		static char newText[252];
		// strcpy(newText, GetText().c_str());
		strcpy_s(newText, 252, GetText().c_str());
		if(ImGui::InputText("##newText",newText, 252))
		{
			SetText(newText);
		}

		static char pathFont[252] = "Assets/Fonts/arial.ttf";
		ImGui::Text("Font"); ImGui::SameLine();
		ImGui::InputText("##pathFont", pathFont, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			std::experimental::filesystem::path path = "Assets/";
			path /= pathFont;

			SetFont(path);
		}
		if(ImGui::DragFloat4("Color", color.ptr(), 1.0f, 0.0f, 255.0f))
		{
			SetColor(color);
		}
		if(ImGui::DragInt("Size", &size, 0.1f, 1, 255))
		{
			SetSize(size);
		}
	}
}

void Text::SetText(const std::string & newText)
{
	if(newText != actualText)
	{
		SetTexture(newText);
		actualText = newText;
	}
}

void Text::SetTexture(const std::string & text)
{
	texture =(Texture*) App->font->RenderFromText(font, text, SDL_Color{(Uint8)color.x, (Uint8)color.y, (Uint8)color.z, (Uint8)color.w });
}

void Text::SetColor(float4 color)
{
	this->color = color;
	SetTexture(actualText);
}

void Text::SetFont(const std::experimental::filesystem::path & path)
{
	font = App->font->LoadFont(path, size);
	if(font != nullptr)
	{
		currentFontPath = path;
	}
	SetText(actualText);
}

void Text::SetSize(int size)
{
	this->size = size;
	UpdateFont();
}

float4 Text::GetColor() const
{
	return color;
}

Texture * Text::GetTexture() const
{
	return texture;
}

const std::string & Text::GetText() const
{
	return actualText;
}

void Text::PreLoad(const nlohmann::json & json)
{
	enabled = json["enabled"];
	actualText = json["actualText"].get<std::string>();
	from_json(json["color"], color);
	size = json["size"];
	currentFontPath = json["currentFontPath"].get<std::string>();
}

void Text::Load(const nlohmann::json & json)
{}

void Text::Save(nlohmann::json & json) const
{
	/*

	Relevant information:

	type
	actualText
	color
	size
	currentFontPath

	*/

	json["type"] = type;
	json["enabled"] = enabled;
	json["actualText"] = actualText;
	nlohmann::json jsonColor;
	to_json(jsonColor, color);
	json["color"] = jsonColor;
	json["size"] = size;
	json["currentFontPath"] = currentFontPath.string();
}

void Text::UpdateFont()
{
	font = App->font->LoadFont(currentFontPath, size);
	SetTexture(actualText);
}