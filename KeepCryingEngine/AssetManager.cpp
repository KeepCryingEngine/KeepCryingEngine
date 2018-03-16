#include "AssetManager.h"

#include <assert.h>

using namespace std;

template <typename K, typename T>
AssetManager<K,T>::AssetManager()
{
}

template <typename K, typename T>
AssetManager<K,T>::~AssetManager()
{
}

template <typename K, typename T>
T* AssetManager<K,T>::GetAsset(const K& identifier)
{
	T* asset = nullptr;
	typename map<K, T*>::iterator assetIt = assets.find(identifier);
	if (assetIt != assets.end())
	{
		asset = assetIt->second;
		Subscribe(asset);
	}
	else
	{
		asset = Load(path);
		if (asset != nullptr)
		{
			Register(asset);
		}
	}

	return asset;
}

template <typename K, typename T>
void AssetManager<K,T>::Subscribe(T * asset)
{
	assert(asset != nullptr);
	map<T*, unsigned int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != assetUsage.end());
	unsigned int &assetUsageCounter = usageIt->second;
	assetUsageCounter += 1;
}

template <typename K, typename T>
void AssetManager<K,T>::Release(T * asset)
{
	assert(asset != nullptr);

	map<T*,unsigned int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != assetUsage.end());
	
	unsigned int &assetUsageCounter = usageIt->second;

	assetUsageCounter  -= 1;
	
	if (assetUsageCounter == 0)
	{
		assetUsage.erase(usageIt);
		map<K, T*>::const_iterator assetIt = assets.find(asset->Path());
		assert(assetIt != assets.cend());

		Unload(asset);
		assets.erase(assetIt);
	}
}

template <typename K, typename T>
size_t AssetManager<K,T>::Size() const
{
	return assets.size();
}

template <typename K, typename T>
void AssetManager<K,T>::Register(T * asset)
{
	assert(asset);
	assert(assets.find(asset->Path()) == assets.end());
	assert(assetUsage.find(asset) == assetUsage.end());

	assets[asset->Path()] = asset;
	assetUsage[asset] = 1;
}