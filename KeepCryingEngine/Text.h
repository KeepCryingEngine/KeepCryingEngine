#ifndef _TEXT_H_
#define _TEXT_H_

#include "Component.h"

#include <SDL_ttf.h>

#include <float4.h>
#include <experimental/filesystem>

class Texture;

class Text : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Text;

	Text();
	~Text();

	void DrawUI() override;

	void SetText(const std::string& newText);
	void SetTexture(const std::string& text);
	void SetColor(float4 color);

	float4 GetColor() const;
	Texture * GetTexture() const;
	const std::string& GetText() const;

private:
	std::string actualText= "Text";
	Texture * texture = nullptr;
	float4 color = float4().zero;
	const TTF_Font* font;
};

#endif // !_TEXT_H_

