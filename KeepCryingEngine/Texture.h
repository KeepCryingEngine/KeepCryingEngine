#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <GL/glew.h>
#include <experimental/filesystem>

#include "Globals.h"
#include "Asset.h"

struct TextureConfiguration {
	GLenum textureType = GL_TEXTURE_2D;
	GLint wrapModeS = GL_CLAMP;
	GLint wrapModeT = GL_CLAMP;
	GLint magFilterMode = GL_LINEAR;
	GLint minFilterMode = GL_LINEAR;
	bool mipMap = true;
	bool anisotropicFilter = true;
};

struct TextureIdentifier
{
	std::experimental::filesystem::path path;

	bool operator<(const TextureIdentifier& other) const;
};

class Texture : public Asset<TextureIdentifier>
{
public:
	Texture(GLuint textureId, const TextureConfiguration& textureConfiguration, uint size, const TextureIdentifier& identifier);

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

	uint GetSize() const;

	const TextureConfiguration& GetTextureConfiguration() const;
	void SetTextureConfiguration(const TextureConfiguration& textureConfiguration);

private:
	GLuint textureId;

	uint size = 0;

	TextureConfiguration textureConfiguration;
};

#endif // !_TEXTURE_H_