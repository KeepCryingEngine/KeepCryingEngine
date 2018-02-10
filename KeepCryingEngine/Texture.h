#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "ModuleTexture.h"

struct TextureConfiguration {
	GLenum textureType = GL_TEXTURE_2D;
	GLint wrapModeS = GL_CLAMP;
	GLint wrapModeT = GL_CLAMP;
	GLint magFilterMode = GL_LINEAR;
	GLint minFilterMode = GL_LINEAR;
	bool mipMap = true;
	bool anisotropicFilter = true;
};

class Texture
{
public:
	Texture(GLuint textureId);
	Texture(GLuint textureId, const TextureConfiguration& textureConfiguration);

	virtual ~Texture();

	GLuint GetId() const;

	GLint GetWrapModeS() const;
	void SetWrapModeS(GLint wrapModeS);

	GLint GetWrapModeT() const;
	void SetWrapModeT(GLint wrapModeT);

	GLint GetMagFilterMode() const;
	void SetMagFilterMode(GLint magFilterMode);

	GLint GetMinFilterMode() const;
	void SetMinFilterMode(GLint minFilterMode);

	bool GetMipMap() const;
	void SetMipMap(bool mipMap);

	bool GetAnisotropicFilter() const;
	void SetAnisotropicFilter(bool anisotropicFilter);

	const TextureConfiguration& GetTextureConfiguration() const;
	void SetTextureConfiguration(const TextureConfiguration& textureConfiguration);

private:
	GLuint textureId;

	TextureConfiguration textureConfiguration;
};

#endif // !_TEXTURE_H_