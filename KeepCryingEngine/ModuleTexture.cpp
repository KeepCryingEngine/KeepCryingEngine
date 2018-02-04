#include "ModuleTexture.h"

#include <DevIL.h>

const uint ModuleTexture::CHECKERS_HEIGHT = 128;
const uint ModuleTexture::CHECKERS_WIDTH = 128;

ModuleTexture::ModuleTexture()
{ }

ModuleTexture::~ModuleTexture()
{ }

uint ModuleTexture::LoadCheckerTexture()
{
	if(checkerTexture != 0)
	{
		return checkerTexture;
	}

	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	
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

	glGenTextures(1, &checkerTexture);
	glBindTexture(GL_TEXTURE_2D, checkerTexture);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Set texture interpolation method to use linear interpolation (no MIPMAPS)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glBindTexture(GL_TEXTURE_2D, 0);

	return checkerTexture;
}

uint ModuleTexture::LoadTexture(const char* texturePath) const
{
	ILuint imageID; // Create an image ID as a ULuint

	GLuint textureID; // Create a texture ID as a GLuint

	ILboolean success; // Create a flag to keep track of success/failure

	ILenum error; // Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); // Generate the image ID

	ilBindImage(imageID); // Bind the image

	success = ilLoadImage(texturePath); // Load the image file

	// If we managed to load the image, then we can start to do things with it...
	if(success)
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
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if(!success)
		{
			error = ilGetError();
			LOG_DEBUG("Image conversion failed - IL reports error: %u - %s", error, iluErrorString(error));
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		if(magFilter)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);
		}

		if(minFilter)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
		}

		if(mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, mipmap);
		}

		if(anisotropicFilter)
		{
			GLfloat maxAniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

			glSamplerParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
		}

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, // Type of texture
			0, // Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT), // Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH), // Image width
			ilGetInteger(IL_IMAGE_HEIGHT), // Image height
			0, // Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT), // Format of image pixel data
			GL_UNSIGNED_BYTE, // Image data type
			ilGetData()); // The actual image data itself
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		LOG_DEBUG("Image load failed - IL reports error: %u - %s", error, iluErrorString(error));
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	return (uint)textureID; // Return the GLuint to the texture so you can use it!
}

uint ModuleTexture::getWrapModeS() const
{
	return wrapModeS;
}

void ModuleTexture::setWrapModeS(uint wrapModeS)
{
	this->wrapModeS = wrapModeS;
}

uint ModuleTexture::getWrapModeT() const
{
	return wrapModeT;
}

void ModuleTexture::setWrapModeT(uint wrapModeT)
{
	this->wrapModeT = wrapModeT;
}

bool ModuleTexture::getMagFilter() const
{
	return magFilter;
}

void ModuleTexture::setMagFilter(bool magFilter)
{
	this->magFilter = magFilter;
}

bool ModuleTexture::getMinFilter() const
{
	return minFilter;
}

void ModuleTexture::setMinFilter(bool minFilter)
{
	this->minFilter = minFilter;
}

uint ModuleTexture::getMagFilterMode() const
{
	return magFilterMode;
}

void ModuleTexture::setMagFilterMode(uint magFilterMode)
{
	this->magFilterMode = magFilterMode;
}

uint ModuleTexture::getMinFilterMode() const
{
	return minFilterMode;
}

void ModuleTexture::setMinFilterMode(uint minFilterMode)
{
	this->minFilterMode = minFilterMode;
}

bool ModuleTexture::getMipmap() const
{
	return mipmap;
}

void ModuleTexture::setMipmap(bool mipMap)
{
	this->mipmap = mipmap;
}

bool ModuleTexture::getAnisotropicFilter() const
{
	return anisotropicFilter;
}

void ModuleTexture::setAnisotropicFilter(bool anisotropicFilter)
{
	this->anisotropicFilter = anisotropicFilter;
}