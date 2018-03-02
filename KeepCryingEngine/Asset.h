#ifndef _ASSET_H_
#define _ASSET_H_

#include <experimental/filesystem>

enum class AssetType {
	Audio,
	Mesh,
	Animation,
	Texture
};

class Asset
{
public:
	

	Asset(AssetType type);
	virtual ~Asset();

	const std::experimental::filesystem::path& Path() const;
	AssetType Type() const;

private:
	std::experimental::filesystem::path path;
	AssetType type;

};

#endif