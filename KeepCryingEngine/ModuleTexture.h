#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include <GL/glew.h>

#include "Module.h"
#include "Texture.h"

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	virtual ~ModuleTexture();

	bool Start() override;
	bool CleanUp() override;

	Texture * LoadTexture(const char* texturePath, const TextureConfiguration& textureConfiguration) const;
	Texture * LoadTexture(const char* texturePath) const;

	Texture* GetCheckerTexture() const;

private:
	void SetUpCheckerTexture();

private:
	TextureConfiguration loadingTextureConfiguration;

	Texture * checkerTexture = nullptr;

	static const uint CHECKERS_HEIGHT;
	static const uint CHECKERS_WIDTH;
};

#endif // !_MODULETEXTURE_H_