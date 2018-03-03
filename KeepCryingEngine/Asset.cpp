#include "Asset.h"

Asset::Asset(const std::experimental::filesystem::path & path, AssetType type) :
	path(path),
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
