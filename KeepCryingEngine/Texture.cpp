#include "Texture.h"


Texture::Texture(GLuint textureId) :
	textureId(textureId)
{
	SetTextureConfiguration(textureConfiguration);
}

Texture::Texture(GLuint textureId, const TextureConfiguration & textureConfiguration) :
	textureId(textureId)
{
	SetTextureConfiguration(textureConfiguration);
}

Texture::~Texture()
{
}

GLuint Texture::GetId() const
{
	return textureId;
}

GLint Texture::GetWrapModeS() const
{
	return textureConfiguration.wrapModeS;
}

void Texture::SetWrapModeS(GLint wrapModeS)
{
	textureConfiguration.wrapModeS = wrapModeS;
	glBindTexture(textureConfiguration.textureType, textureId);
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_WRAP_S, wrapModeS);
	glBindTexture(textureConfiguration.textureType, 0);
}

GLint Texture::GetWrapModeT() const
{
	return textureConfiguration.wrapModeT;
}

void Texture::SetWrapModeT(GLint wrapModeT)
{
	textureConfiguration.wrapModeT = wrapModeT;
	glBindTexture(textureConfiguration.textureType, textureId);
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_WRAP_T, wrapModeT);
	glBindTexture(textureConfiguration.textureType, 0);
}

GLint Texture::GetMagFilterMode() const
{
	return textureConfiguration.magFilterMode;
}

void Texture::SetMagFilterMode(GLint magFilterMode)
{
	textureConfiguration.magFilterMode = magFilterMode;
	glBindTexture(textureConfiguration.textureType, textureId);
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_MAG_FILTER, magFilterMode);
	glBindTexture(textureConfiguration.textureType, 0);

}

GLint Texture::GetMinFilterMode() const
{
	return textureConfiguration.minFilterMode;
}

void Texture::SetMinFilterMode(GLint minFilterMode)
{
	textureConfiguration.minFilterMode = minFilterMode;
	glBindTexture(textureConfiguration.textureType, textureId);
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_MIN_FILTER, minFilterMode);
	glBindTexture(textureConfiguration.textureType, 0);

}

bool Texture::GetMipMap() const
{
	return textureConfiguration.mipMap;
}

void Texture::SetMipMap(bool mipMap)
{
	textureConfiguration.mipMap = mipMap;
	if (mipMap)
	{
		glBindTexture(textureConfiguration.textureType, textureId);
		glTexParameteri(textureConfiguration.textureType, GL_GENERATE_MIPMAP, mipMap);
		glBindTexture(textureConfiguration.textureType, 0);
	}
}

bool Texture::GetAnisotropicFilter() const
{
	return textureConfiguration.anisotropicFilter;
}

void Texture::SetAnisotropicFilter(bool anisotropicFilter)
{
	textureConfiguration.anisotropicFilter = anisotropicFilter;
	if (anisotropicFilter)
	{
		glBindTexture(textureConfiguration.textureType, textureId);
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glSamplerParameterf(textureConfiguration.textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
		glBindTexture(textureConfiguration.textureType, 0);
	}
}

const TextureConfiguration & Texture::GetTextureConfiguration() const
{
	return textureConfiguration;
}

void Texture::SetTextureConfiguration(const TextureConfiguration & textureConfiguration)
{
	this->textureConfiguration = textureConfiguration;
	glBindTexture(textureConfiguration.textureType, textureId);

	//Set vertical and horizontal wrap mode
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_WRAP_S, textureConfiguration.wrapModeS);
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_WRAP_T, textureConfiguration.wrapModeT);

	//Set magnification and minification mode
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_MAG_FILTER, textureConfiguration.magFilterMode);
	glTexParameteri(textureConfiguration.textureType, GL_TEXTURE_MIN_FILTER, textureConfiguration.minFilterMode);

	if (textureConfiguration.mipMap)
	{
		glBindTexture(textureConfiguration.textureType, textureId);
		glTexParameteri(textureConfiguration.textureType, GL_GENERATE_MIPMAP, textureConfiguration.mipMap);
		glBindTexture(textureConfiguration.textureType, 0);
	}
	
	if (textureConfiguration.anisotropicFilter)
	{
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
		glSamplerParameterf(textureConfiguration.textureType, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
	}

	glBindTexture(textureConfiguration.textureType, 0);
}