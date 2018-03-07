#include "ModuleFont.h"

#include "ModuleTexture.h"

using namespace std;
using namespace std::experimental::filesystem;

ModuleFont::ModuleFont()
{ }

ModuleFont::~ModuleFont()
{ }

bool ModuleFont::Init()
{
	return TTF_Init() == 0;
}

bool ModuleFont::CleanUp()
{
	TTF_Quit();

	return true;
}

const TTF_Font* ModuleFont::LoadFont(const path& fontPath, int size)
{
	map<pair<path, int>, TTF_Font*>::iterator it = fonts.find(make_pair(fontPath, size));

	if(it != fonts.end())
	{
		return it->second;
	}

	TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), size);

	if(font != nullptr)
	{
		fonts[make_pair(fontPath, size)] = font;
	}

	return font;
}

const Texture* ModuleFont::RenderFromText(const TTF_Font* font, const string& message, const SDL_Color& color) const
{
	if(font == nullptr)
	{
		return nullptr;
	}

	SDL_Surface* sFont = TTF_RenderText_Blended((TTF_Font*)font, message.c_str(), color);

	const Texture* tFont = CreateTextureFromSurface(sFont);

	SDL_FreeSurface(sFont);

	return tFont;
}

const Texture* ModuleFont::CreateTextureFromSurface(const SDL_Surface* surface) const
{
	if(surface == nullptr)
	{
		return nullptr;
	}

	const uint w = surface->w;
	const uint h = surface->h;

	GLuint tId = 0;
	glGenTextures(1, &tId);

	glBindTexture(GL_TEXTURE_2D, tId);

	int textureFormat;
	int colors = surface->format->BytesPerPixel;

	if(colors == 4)
	{
		textureFormat = surface->format->Rmask == 0x000000ff ? GL_RGBA : GL_BGRA;
	}
	else
	{
		textureFormat = surface->format->Rmask == 0x000000ff ? GL_RGB : GL_BGR;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, colors, w, h, 0, textureFormat, GL_UNSIGNED_BYTE, surface->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	TextureConfiguration tConfig;
	tConfig.textureType = GL_TEXTURE_2D;
	tConfig.mipMap = false;
	tConfig.magFilterMode = GL_LINEAR;
	tConfig.minFilterMode = GL_LINEAR;
	tConfig.wrapModeS = GL_CLAMP;
	tConfig.wrapModeT = GL_CLAMP;
	tConfig.anisotropicFilter = true;

	return new Texture(tId, tConfig, w * h * surface->format->BytesPerPixel);
}