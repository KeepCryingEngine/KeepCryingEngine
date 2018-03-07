#include "ModuleFont.h"

#include <SDL.h>

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTexture.h"

using namespace std;

ModuleFont::ModuleFont()
{ }

ModuleFont::~ModuleFont()
{ }

bool ModuleFont::Init()
{
	TTF_Init();

	int size = 24;
	const char* message = "Gerard es muy atractivo";
	SDL_Color color = { 0, 0, 255, 1 };

	font = TTF_OpenFont("Assets/Fonts/arial.ttf", size);
	SDL_Surface * sFont = TTF_RenderText_Solid(font, message, color);

	tFont = CreateTextureFromFontSurface(sFont);

	SDL_FreeSurface(sFont);
	return true;
}

bool ModuleFont::CleanUp()
{
	TTF_CloseFont(font);
	return true;
}

Texture * ModuleFont::CreateTextureFromFontSurface(SDL_Surface * fontSurface)
{
	const uint fontTextHeight = fontSurface->h;
	const uint fontTextWidth = fontSurface->w;

	//GLubyte checkImage[fontTextHeight][fontTextWidth][4];

	////Fill texture data
	//for(int i = 0; i < CHECKERS_HEIGHT; ++i)
	//{
	//	for(int j = 0; j < CHECKERS_WIDTH; ++j)
	//	{
	//		int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;

	//		checkImage[i][j][0] = (GLubyte)c;
	//		checkImage[i][j][1] = (GLubyte)c;
	//		checkImage[i][j][2] = (GLubyte)c;
	//		checkImage[i][j][3] = (GLubyte)255;
	//	}
	//}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint checkerTextureId = 0;
	glGenTextures(1, &checkerTextureId);

	glBindTexture(GL_TEXTURE_2D, checkerTextureId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fontTextWidth, fontTextHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, fontSurface->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);

	TextureConfiguration textureConfiguration;
	textureConfiguration.textureType = GL_TEXTURE_2D;
	textureConfiguration.mipMap = false;
	textureConfiguration.magFilterMode = GL_LINEAR;
	textureConfiguration.minFilterMode = GL_LINEAR;
	textureConfiguration.wrapModeS = GL_CLAMP;
	textureConfiguration.wrapModeT = GL_CLAMP;
	textureConfiguration.anisotropicFilter = true;

	return new Texture(checkerTextureId, textureConfiguration, sizeof(fontSurface->pixels));
}
