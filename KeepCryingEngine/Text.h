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

	void Awake()override;

	void DrawUI() override;

	std::vector<Component::Type> GetNeededComponents() const override;
	void SetText(const std::string& newText);
	void SetTexture(const std::string& text);
	void SetColor(float4 color);
	void SetFont(const std::experimental::filesystem::path & path);
	void SetSize(float size);

	float4 GetColor() const;
	Texture * GetTexture() const;
	const std::string& GetText() const;

private:
	void UpdateFont();

private:
	std::string actualText= "Text";
	Texture * texture = nullptr;
	float4 color = float4().one * 255;
	const TTF_Font* font;
	float size = 20;
	std::experimental::filesystem::path currentFontPath = "";
};

#endif // !_TEXT_H_

