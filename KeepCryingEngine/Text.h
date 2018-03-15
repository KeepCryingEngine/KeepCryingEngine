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

	void Start()override;

	void DrawUI() override;

	std::vector<Component::Type> GetNeededComponents() const override;
	void SetText(const std::string& newText);
	void SetTexture(const std::string& text);
	void SetColor(float4 color);
	void SetFont(const std::experimental::filesystem::path & path);
	void SetSize(int size);

	float4 GetColor() const;
	Texture * GetTexture() const;
	const std::string& GetText() const;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	void UpdateFont();

private:
	std::string actualText= "Text";
	Texture * texture = nullptr;
	float4 color = float4().one * 255;
	const TTF_Font* font;
	int size = 20;
	std::experimental::filesystem::path currentFontPath = "Assets/Fonts/arial.ttf";
};

#endif // !_TEXT_H_

