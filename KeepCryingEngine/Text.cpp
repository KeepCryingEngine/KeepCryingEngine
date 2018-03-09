#include "Text.h"

Text::Text(): Component(Text::TYPE)
{}

Text::~Text()
{}

void Text::DrawUI()
{
	static char newText[252] = "Text";
	ImGui::Text("Disabled"); ImGui::SameLine();
	if(ImGui::InputText("##buttonDisabledTexture", newText, 252))
	{
		SetText(newText);
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
{}

void Text::SetColor(float4 color)
{
	this->color = color;
	SetTexture(actualText);
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
