#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include <set>
#include <map>
#include <GL/glew.h>
#include <string>

#include "AssetManager.cpp"
#include "Texture.h"

class ModuleTexture : public AssetManager<Texture>
{
public:
	ModuleTexture();
	virtual ~ModuleTexture();

	bool Start() override;

	Texture* GetCheckerTexture();
	uint TotalTextureSize() const;

	const std::set<std::string>& TexturePaths() const;

protected:
	Texture * Load(const std::string& path) override;
	void Unload(Texture* texture) override;

private:
	void SetUpCheckerTexture();
	Texture* LoadTextureDevil(const std::string & texturePath, const TextureConfiguration& textureConfiguration) const;
	
private:
	TextureConfiguration loadingTextureConfiguration;
	Texture* checkerTexture = nullptr;
	uint totalSize = 0;
	std::set<std::string> texturePaths;
};

#endif // !_MODULETEXTURE_H_