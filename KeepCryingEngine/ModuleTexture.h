#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include "Module.h"

class ModuleTexture : Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	uint LoadTexture(const char* theFileName, ILinfo* imageInfo);
private:
	ILinfo * currentImageInfo = nullptr;

	ILinfo* lenaImageInfo = nullptr;
	ILinfo* rockImageInfo = nullptr;
	ILinfo* exodiaImageInfo = nullptr;

	uint wrapModeS = 0;
	uint wrapModeT = 0;
	bool magFilter = true;
	bool minFilter = true;
	uint magFilterMode = 0;
	uint minFilterMode = 0;
	bool mipmap = false;
	bool anisotropicFilter = false;
};

#endif // !_MODULETEXTURE_H_

