#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include <map>
#include <string>

template <typename T>
class AssetManager
{
public:
	AssetManager();
	virtual ~AssetManager();

	T* GetAsset(const std::string& path);

	void Unload(T* asset);

	template <typename T>
	T* DoLoad(const std::string& path) = 0;
private:
	typename std::map<std::string, T*>::const_iterator FindAssetIterator(T * asset) const;

private:
	std::map<std::string, T*> assets;
	std::map<T*, int> assetUsage;
};

#endif