#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include "Module.h"

#include <map>
#include <experimental/filesystem>
#include <assert.h>

#include "Asset.h"


template <typename K, typename T>
class AssetManager : public Module
{
	static_assert(std::is_base_of<Asset<K>, T>::value, "T must be a child of Asset<K>");
public:
	AssetManager();
	virtual ~AssetManager();

	T* GetAsset(const K& identifier);
	void Subscribe(T* asset);

	void Release(T* asset);

	size_t Size() const;

protected:
	void Register(T* asset);
	virtual T * Load(const K& identifier) = 0;
	virtual void Unload(T* asset) = 0;

private:
	std::map<K, T*> assets;
	std::map<T*,unsigned int> assetUsage;
};

template <typename K, typename T>
inline AssetManager<K, T>::AssetManager()
{
}

template <typename K, typename T>
inline AssetManager<K, T>::~AssetManager()
{
}

template <typename K, typename T>
inline T* AssetManager<K, T>::GetAsset(const K& identifier)
{
	T* asset = nullptr;
	typename std::map<K, T*>::iterator assetIt = assets.find(identifier);
	if (assetIt != assets.end())
	{
		asset = assetIt->second;
		Subscribe(asset);
	}
	else
	{
		asset = Load(identifier);
		if (asset != nullptr)
		{
			Register(asset);
		}
	}

	return asset;
}

template <typename K, typename T>
inline void AssetManager<K, T>::Subscribe(T * asset)
{
	assert(asset != nullptr);
	std::map<T*, unsigned int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != assetUsage.end());
	unsigned int &assetUsageCounter = usageIt->second;
	assetUsageCounter += 1;
}

template <typename K, typename T>
inline void AssetManager<K, T>::Release(T * asset)
{
	assert(asset != nullptr);

	typename map<T*, unsigned int>::iterator usageIt = assetUsage.find(asset);
	assert(usageIt != assetUsage.end());

	unsigned int &assetUsageCounter = usageIt->second;

	assetUsageCounter -= 1;

	if (assetUsageCounter == 0)
	{
		assetUsage.erase(usageIt);
		map<K, T*>::const_iterator assetIt = assets.find(asset->Identifier());
		assert(assetIt != assets.cend());

		Unload(asset);
		assets.erase(assetIt);
	}
}

template <typename K, typename T>
inline size_t AssetManager<K, T>::Size() const
{
	return assets.size();
}

template <typename K, typename T>
inline void AssetManager<K, T>::Register(T * asset)
{
	assert(asset != nullptr);

	typename std::map<K, T*>::iterator assetIt = assets.find(asset->Identifier());

	assert(assetIt == assets.end());
	assert(assetUsage.find(asset) == assetUsage.end());

	assets[asset->Identifier()] = asset;
	assetUsage[asset] = 1;
}

#endif