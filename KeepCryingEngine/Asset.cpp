#include "Asset.h"

<<<<<<< HEAD
template <typename K>
Asset::Asset<K>(const K& identifier, AssetType type) :
	identifier(identifier),
=======
Asset::Asset(const std::experimental::filesystem::path & path, AssetType type) :
	path(path),
>>>>>>> asset
	type(type)
{
}

Asset::~Asset()
{
}

const std::experimental::filesystem::path & Asset::Path() const
{
	return path;
}

AssetType Asset::Type() const
{
	return Type();
}
