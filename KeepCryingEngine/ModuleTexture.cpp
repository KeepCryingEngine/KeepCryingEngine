#include "ModuleTexture.h"

#include <DevIL.h>
#include <string>

#include <assert.h>

using namespace std;

ModuleTexture::ModuleTexture()
{ }

ModuleTexture::~ModuleTexture()
{ }

bool ModuleTexture::Start()
{
	SetUpCheckerTexture();

	return true;
}

void ModuleTexture::SetUpCheckerTexture()
{
	const uint CHECKERS_HEIGHT = 128;
	const uint CHECKERS_WIDTH = 128;

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

	checkerTexture = new Texture(checkerTextureId, textureConfiguration, sizeof(checkImage));

	const string checkerTexturePath = "CHECKER_TEXTURE";
	Register(checkerTexturePath, checkerTexture);
	texturePaths.insert(checkerTexturePath);
	totalSize += checkerTexture->GetSize();
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

		texture = new Texture(textureId, textureConfiguration, imageInfo.SizeOfData);
	}
	else // If we failed to open the image file in the first place...
	{
		ILenum error = ilGetError();
		LOG_DEBUG("Image load failed - IL reports error: %u - %s", error, iluErrorString(error));
	}

	return texture;
}

Texture * ModuleTexture::GetCheckerTexture()
{
	Subscribe(checkerTexture);
	return checkerTexture;
}

uint ModuleTexture::TotalTextureSize() const
{
	return totalSize;
}

const set<string>& ModuleTexture::TexturePaths() const
{
	return texturePaths;
}

Texture * ModuleTexture::Load(const std::string & path)
{
	Texture * texture = LoadTextureDevil(path, loadingTextureConfiguration);
	
	if (texture)
	{
		texturePaths.insert(path);
		totalSize += texture->GetSize();
	}

	return texture;
}

void ModuleTexture::Unload(Texture * texture)
{
	totalSize -= texture->GetSize();
	
	const string& path = GetPath(texture);
	texturePaths.erase(path);

	delete texture;
}
