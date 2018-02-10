#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include <GL/glew.h>

#include "Module.h"

class Texture;

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	virtual ~ModuleTexture();

	uint LoadCheckerTexture();
	Texture * LoadTexture(const char* texturePath) const;

	uint getWrapModeS() const;
	void setWrapModeS(uint wrapModeS);

	uint getWrapModeT() const;
	void setWrapModeT(uint wrapModeT);

	bool getMagFilter() const;
	void setMagFilter(bool magFilter);

	bool getMinFilter() const;
	void setMinFilter(bool minFilter);

	uint getMagFilterMode() const;
	void setMagFilterMode(uint magFilterMode);

	uint getMinFilterMode() const;
	void setMinFilterMode(uint minFilterMode);

	bool getMipmap() const;
	void setMipmap(bool mipMap);

	bool getAnisotropicFilter() const;
	void setAnisotropicFilter(bool anisotropicFilter);

private:
	uint checkerTexture = 0;

	uint wrapModeS = GL_CLAMP;
	uint wrapModeT = GL_CLAMP;
	bool magFilter = true;
	bool minFilter = true;
	uint magFilterMode = GL_LINEAR;
	uint minFilterMode = GL_LINEAR;
	bool mipmap = false;
	bool anisotropicFilter = false;

	static const uint CHECKERS_HEIGHT;
	static const uint CHECKERS_WIDTH;
};

#endif // !_MODULETEXTURE_H_