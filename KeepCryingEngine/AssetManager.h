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

	template <typename T>
	T* Load(const std::string& path);

	template <typename T>
	void Unload(T* asset);

	template <typename T>
	virtual T* DoLoad(const std::string& path) = 0;

private:
	template <typename T>
	std::map<std::string, T*>::iterator FindAssetIterator(T* asset) const;

private:
	std::map<std::string, T*> assets;
	std::map<T*, int> assetUsage;
};

#endif