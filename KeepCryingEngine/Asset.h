#ifndef _ASSET_H_
#define _ASSET_H_

#include <experimental/filesystem>

enum class AssetType {
	Audio,
	Mesh,
	Animation,
	Texture
};

<<<<<<< HEAD
template <typename K>
=======
>>>>>>> asset
class Asset
{
public:
	

<<<<<<< HEAD
	Asset(const K& identifier, AssetType type);
	virtual ~Asset();

	const K& Identifier() const;
	AssetType Type() const;

private:
	AssetType type;
	K identifier;
=======
	Asset(const std::experimental::filesystem::path& path, AssetType type);
	virtual ~Asset();

	const std::experimental::filesystem::path& Path() const;
	AssetType Type() const;

private:
	std::experimental::filesystem::path path;
	AssetType type;

>>>>>>> asset
};

#endif