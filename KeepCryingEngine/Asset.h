#ifndef _ASSET_H_
#define _ASSET_H_

#include <experimental/filesystem>

enum class AssetType {
	Audio,
	Mesh,
	Animation,
	Texture
};

template <typename K>
class Asset
{
public:
	

	Asset(const K& identifier, AssetType type);
	virtual ~Asset();

	const K& Identifier() const;
	AssetType Type() const;

private:
	AssetType type;
	K identifier;
};

#endif