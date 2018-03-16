#include "ModuleFont.h"

#include "ModuleTexture.h"

#include <DevIL.h>
#include <sstream>

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

static void split(const string& s, char delim, vector<string>& elems)
{
	string item;
	stringstream ss(s);
	
	while(getline(ss, item, delim))
	{
		elems.push_back(item);
	}
}

const Texture* ModuleFont::RenderFromText(const TTF_Font* font, const string& message, const SDL_Color& color, int minWrapW) const
{
	if(font == nullptr)
	{
		return nullptr;
	}

	SDL_Surface* sFont;

	if(minWrapW >= 0)
	{
		int wrapLongestWordW = 0;

		vector<string> words;
		split(message, ' ', words);

		for(const string& word : words)
		{
			int w;
			TTF_SizeText((TTF_Font*)font, word.c_str(), &w, nullptr);

			if(w > wrapLongestWordW)
			{
				wrapLongestWordW = w;
			}
		}

		int wrapLength = max(minWrapW, wrapLongestWordW);

		sFont = TTF_RenderText_Blended_Wrapped((TTF_Font*)font, message.c_str(), color, wrapLength);
	}
	else
	{
		sFont = TTF_RenderText_Blended((TTF_Font*)font, message.c_str(), color);
	}

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
	
	int textureFormat;
	int colors = surface->format->BytesPerPixel;

	if(colors == 4)
	{
		textureFormat = surface->format->Rmask == 0x000000FF ? IL_RGBA : IL_BGRA;
	}
	else
	{
		textureFormat = surface->format->Rmask == 0x000000FF ? IL_RGB : IL_BGR;
	}
	
	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);

	ilTexImage(surface->w, surface->h, 1, colors, textureFormat, IL_UNSIGNED_BYTE, surface->pixels);

	//iluMirror();
	iluFlipImage();

	GLuint tId = 0;
	glGenTextures(1, &tId);
	glBindTexture(GL_TEXTURE_2D, tId);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		ilGetInteger(IL_IMAGE_BPP),
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT),
		0,
		ilGetInteger(IL_IMAGE_FORMAT),
		ilGetInteger(IL_IMAGE_TYPE),
		ilGetData()
	);

	int tSize = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);

	glBindTexture(GL_TEXTURE_2D, 0);

	ilDeleteImages(1, &imageId);

	ilBindImage(0);

	TextureConfiguration tConfig;
	tConfig.textureType = GL_TEXTURE_2D;
	tConfig.mipMap = false;
	tConfig.magFilterMode = GL_LINEAR;
	tConfig.minFilterMode = GL_LINEAR;
	tConfig.wrapModeS = GL_CLAMP;
	tConfig.wrapModeT = GL_CLAMP;
	tConfig.anisotropicFilter = true;

	TextureIdentifier textureIdentifier = { "no_path_for_a_font" };
	return new Texture(tId, tConfig, tSize, textureIdentifier);
}