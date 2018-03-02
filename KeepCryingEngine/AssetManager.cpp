#include "AssetManager.h"

#include <assert.h>

using namespace std;

template <typename T>
AssetManager<T>::AssetManager()
{
}

template <typename T>
AssetManager<T>::~AssetManager()
{
}

template <typename T>
T* AssetManager<T>::GetAsset(const std::experimental::filesystem::path& path)
{
	T* asset = nullptr;
	typename map<std::experimental::filesystem::path, T*>::iterator assetIt = assets.find(path);
	if (assetIt != assets.end())
	{
		asset = assetIt->second;
		Subscribe(asset);
	}
	else
	{
		asset = Load(path);
		if (asset)
		{
			Register(path, asset);
		}
	}

	return asset;
}

template<typename T>
void AssetManager<T>::Subscribe(T * asset)
{
	assert(asset);
	map<T*, unsigned int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != assetUsage.end());
	unsigned int &assetUsageCounter = usageIt->second;
	assetUsageCounter += 1;
}

template<typename T>
void AssetManager<T>::Release(T * asset)
{
	assert(asset);

	map<T*,unsigned int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != assetUsage.end());
	
	unsigned int &assetUsageCounter = usageIt->second;

	assetUsageCounter  -= 1;
	
	if (assetUsageCounter == 0)
	{
		assetUsage.erase(usageIt);
		map<std::experimental::filesystem::path, T*>::const_iterator assetIt = FindAssetIteratorByReference(asset);
		assert(assetIt != assets.cend());

		Unload(asset);
		assets.erase(assetIt);
	}
}

template<typename T>
size_t AssetManager<T>::Size() const
{
	return assets.size();
}

template<typename T>
const std::experimental::filesystem::path & AssetManager<T>::GetPath(T * asset) const
{
	assert(asset);
	map<std::experimental::filesystem::path, T*>::const_iterator assetIt = FindAssetIteratorByReference(asset);
	assert(assetIt != assets.cend());

	return assetIt->first;
}

template<typename T>
void AssetManager<T>::Register(const std::experimental::filesystem::path& path, T * asset)
{
	assert(asset);
	assert(assets.find(path) == assets.end());
	assert(assetUsage.find(asset) == assetUsage.end());

	assets[path] = asset;
	assetUsage[asset] = 1;
}

template<typename T>
typename map<std::experimental::filesystem::path, T*>::const_iterator AssetManager<T>::FindAssetIteratorByReference(T * asset) const
{
	assert(asset);
	typename map<std::experimental::filesystem::path, T*>::const_iterator assetIt = assets.cbegin();
	while (assetIt != assets.cend() && assetIt->second != asset)
	{
		++assetIt;
	}
	return assetIt;
}