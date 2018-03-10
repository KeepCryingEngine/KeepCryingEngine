#include "Text.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleFont.h"

Text::Text(): Component(Text::TYPE)
{}

Text::~Text()
{}

void Text::Awake()
{
	font = App->font->LoadFont("Assets/Fonts/arial.ttf", 69);
	currentFontPath = "Assets/Fonts/arial.ttf";
	SetTexture(actualText);
}

void Text::DrawUI()
{
	if(ImGui::CollapsingHeader("Text"))
	{
		static bool uiEnable = enabled;
		ImGui::Checkbox("Active", &uiEnable); ImGui::SameLine();
		SetEnable(uiEnable);
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
		static char newText[252];
		strcpy(newText, GetText().c_str());
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
		if(ImGui::DragFloat("Size", &size, 0.1f, 1.0f, 255.0f))
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

void Text::SetSize(float size)
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

void Text::UpdateFont()
{
	font = App->font->LoadFont(currentFontPath, size);
	SetTexture(actualText);
}
