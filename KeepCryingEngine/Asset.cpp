#include "Asset.h"

template <typename K>
Asset::Asset<K>(const K& identifier, AssetType type) :
	identifier(identifier),
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
