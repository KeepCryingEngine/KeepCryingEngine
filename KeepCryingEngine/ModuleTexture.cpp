#include "ModuleTexture.h"

#include <DevIL.h>
#include <string>

#include <assert.h>

using namespace std;

const uint ModuleTexture::CHECKERS_HEIGHT = 128;
const uint ModuleTexture::CHECKERS_WIDTH = 128;

ModuleTexture::ModuleTexture()
{ }

ModuleTexture::~ModuleTexture()
{ }

bool ModuleTexture::Start()
{
	SetUpCheckerTexture();

	return true;
}

bool ModuleTexture::CleanUp()
{
	RELEASE(checkerTexture);

	return true;
}

void ModuleTexture::SetUpCheckerTexture()
{
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	
	//Fill texture data
	for(int i = 0; i < CHECKERS_HEIGHT; ++i)
	{
		for(int j = 0; j < CHECKERS_WIDTH; ++j)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;

			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	GLuint checkerTextureId = 0;
	glGenTextures(1, &checkerTextureId);
	
	glBindTexture(GL_TEXTURE_2D, checkerTextureId);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glBindTexture(GL_TEXTURE_2D, 0);

	TextureConfiguration textureConfiguration;
	textureConfiguration.textureType = GL_TEXTURE_2D;
	textureConfiguration.mipMap = false;
	textureConfiguration.magFilterMode = GL_LINEAR;
	textureConfiguration.minFilterMode = GL_LINEAR;
	textureConfiguration.wrapModeS = GL_CLAMP;
	textureConfiguration.wrapModeT = GL_CLAMP;
	textureConfiguration.anisotropicFilter = true;

	checkerTexture = new Texture(checkerTextureId, textureConfiguration);

	RegisterTexture("CHECKER_TEXTURE", checkerTexture);
}

Texture * ModuleTexture::GetChachedTexure(const string & texturePath) const
{
	Texture * texture = nullptr;
	map<string, Texture*>::const_iterator textureIt = loadedTextures.find(texturePath);

	if(textureIt != loadedTextures.end())
	{
		texture = textureIt->second;
	}

	return texture;
}

Texture * ModuleTexture::LoadTextureDevil(const string & texturePath, const TextureConfiguration& textureConfiguration) const
{
	Texture * texture = nullptr;

	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);

	if(ilLoadImage(texturePath.c_str()))
	{
		ILinfo imageInfo;

		iluGetImageInfo(&imageInfo);

		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		if(imageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		if(!ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE))
		{
			ILenum error = ilGetError();
			LOG_DEBUG("Image conversion failed - IL reports error: %u - %s", error, iluErrorString(error));
		}

		GLuint textureId = 0;

		glGenTextures(1, &textureId);
		glBindTexture(textureConfiguration.textureType, textureId);

		glTexImage2D(textureConfiguration.textureType,
			0,
			ilGetInteger(IL_IMAGE_FORMAT), // Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),
			ilGetInteger(IL_IMAGE_HEIGHT),
			0,
			ilGetInteger(IL_IMAGE_FORMAT), // Format of image pixel data
			GL_UNSIGNED_BYTE, // Image data type
			ilGetData()); // The actual image data itself

		ilDeleteImages(1, &imageId); // Because we have already copied image data into texture data we can release memory used by image.

		texture = new Texture(textureId, textureConfiguration);

		texture->SetSize(imageInfo.SizeOfData);
	}
	else // If we failed to open the image file in the first place...
	{
		ILenum error = ilGetError();
		LOG_DEBUG("Image load failed - IL reports error: %u - %s", error, iluErrorString(error));
	}
	return texture;
}

void ModuleTexture::RegisterTexture(const string & texturePath, Texture * texture)
{
	assert(texture);

	map<string, Texture*>::iterator textureIt = loadedTextures.find(texturePath);
	if(textureIt != loadedTextures.end())
	{
		textureUses[texture] += 1;
	}
	else
	{
		loadedTextures[texturePath] = texture;
		textureUses[texture] = 1;

		totalSize += texture->GetSize();

		texturePaths.insert(texturePath);
	}
}

bool ModuleTexture::UnregisterTexture(Texture * texture)
{
	assert(texture);

	map<Texture*, uint>::iterator textureUsageIt = textureUses.find(texture);
	assert(textureUsageIt != textureUses.end());

	uint &usageCounter = textureUsageIt->second;
	usageCounter -= 1;

	bool toDelete = usageCounter == 0;
	if(toDelete)
	{
		textureUses.erase(textureUsageIt);

		map<string, Texture*>::iterator textureIt = loadedTextures.begin();

		while(textureIt != loadedTextures.end() && textureIt->second != texture)
		{
			++textureIt;
		}

		assert(textureIt != loadedTextures.end());

		texturePaths.erase(textureIt->first);

		loadedTextures.erase(textureIt);

		totalSize -= texture->GetSize();
	}
	return toDelete;
}

Texture * ModuleTexture::LoadTexture(const string& texturePath, const TextureConfiguration & textureConfiguration)
{
	Texture * texture = GetChachedTexure(texturePath);

	if(texture == nullptr)
	{
		texture = LoadTextureDevil(texturePath, textureConfiguration);
	}
	
	if(texture != nullptr)
	{
		RegisterTexture(texturePath, texture);
	}

	return texture;
}

Texture * ModuleTexture::LoadTexture(const string& texturePath)
{
	return LoadTexture(texturePath, loadingTextureConfiguration);
}

void ModuleTexture::UnloadTexture(Texture * texture)
{
	bool toDelete = UnregisterTexture(texture);

	if(toDelete)
	{
		delete texture;
	}
}

Texture * ModuleTexture::GetCheckerTexture() const
{
	return checkerTexture;
}

uint ModuleTexture::GetTextureCount() const
{
	return loadedTextures.size();
}

uint ModuleTexture::GetTextureTotalSize() const
{
	return totalSize;
}

const set<string>& ModuleTexture::GetTexturePaths() const
{
	return texturePaths;
}
