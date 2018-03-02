#include "Asset.h"

Asset::Asset()
{
}

Asset::Asset(AssetType type) :
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
