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
T* AssetManager<T>::GetAsset(const string& path)
{
	T* asset = nullptr;
	typename map<string, T*>::iterator assetIt = assets.find(path);
	if (assetIt != assets.end())
	{
		asset = assetIt->second;
		(*assetUsage[asset]) += 1;
	}
	else
	{
		asset = DoLoad(path);
		if (asset)
		{
			assetUsage[asset] = 1;
		}
	}

	return asset;
}

template<typename T>
void AssetManager<T>::Unload(T * asset)
{
	map<T*, int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != usageIt.end());
	
	(*usageIt)->second -= 1;
	
	if ((*usageIt)->second == 0)
	{
		assetUsage.erase(usageIt);
		map<string, T*>::iterator assetIt = FindAssetIterator(asset);
		assert(assetIt != assets.end());
		assets.erase(assetIt);
	}
}

template<typename T>
typename map<string, T*>::const_iterator AssetManager<T>::FindAssetIterator(T * asset) const
{
	typename map<string, T*>::const_iterator assetIt = assets.begin();
	while (assetIt != assets.end() && (*assetIt)->second != asset)
	{
		++assetIt;
	}
	return assetIt;
}
