#ifndef _MODULEFONT_H_
#define _MODULEFONT_H_

#include <SDL_ttf.h>

#include "Module.h"

struct SDL_Surface;
struct SDL_Texture;
class Texture;

class ModuleFont : public Module
{
public:
	ModuleFont();
	virtual ~ModuleFont();

	bool Init() override;
	bool CleanUp() override;

private:
	Texture * CreateTextureFromFontSurface(SDL_Surface * fontSurface);

private:
	TTF_Font * font = nullptr;
	Texture * tFont = nullptr;
};

#endif