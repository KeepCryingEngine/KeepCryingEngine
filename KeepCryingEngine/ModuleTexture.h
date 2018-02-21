#ifndef _MODULETEXTURE_H_
#define _MODULETEXTURE_H_

#include <set>
#include <map>
#include <GL/glew.h>
#include <string>

#include "Module.h"
#include "Texture.h"

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	virtual ~ModuleTexture();

	bool Start() override;

	Texture* LoadTexture(const std::string& texturePath, const TextureConfiguration& textureConfiguration);
	Texture* LoadTexture(const std::string& texturePath);

	void UnloadTexture(Texture* texture);

	Texture* GetCheckerTexture();

	uint GetTextureCount() const;

	uint GetTextureTotalSize() const;

	const std::set<std::string>& GetTexturePaths() const;

	void SubscribeToTexture(Texture* texture);

private:
	void SetUpCheckerTexture();

	Texture* GetChachedTexure(const std::string & texturePath) const;
	Texture* LoadTextureDevil(const std::string & texturePath, const TextureConfiguration& textureConfiguration) const;
	void RegisterTexture(const std::string& texturePath, Texture* texture);
	
	bool UnregisterTexture(Texture* texture);

private:
	TextureConfiguration loadingTextureConfiguration;

	Texture* checkerTexture = nullptr;

	std::map<std::string, Texture*> loadedTextures;
	std::map<Texture*, uint> textureUses;

	uint totalSize = 0;

	std::set<std::string> texturePaths;

	static const uint CHECKERS_HEIGHT;
	static const uint CHECKERS_WIDTH;
};

#endif // !_MODULETEXTURE_H_