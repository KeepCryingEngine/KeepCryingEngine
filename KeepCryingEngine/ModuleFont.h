#ifndef _MODULEFONT_H_
#define _MODULEFONT_H_

#include <map>
#include <SDL_ttf.h>
#include <experimental/filesystem>

#include "Module.h"

class Texture;

class ModuleFont : public Module
{
public:
	ModuleFont();
	virtual ~ModuleFont();

	bool Init() override;
	bool CleanUp() override;

	const TTF_Font* LoadFont(const std::experimental::filesystem::path& fontPath, int size);

	const Texture* RenderFromText(const TTF_Font* font, const std::string& message, const SDL_Color& color) const;

private:
	const Texture* CreateTextureFromSurface(const SDL_Surface* surface) const;

private:
	std::map<std::pair<std::experimental::filesystem::path, int>, TTF_Font*> fonts;
};

#endif