#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include <map>
#include <string>

#include "Module.h"

template <typename T>
class AssetManager : public Module
{
public:
	AssetManager();
	virtual ~AssetManager();

	T* GetAsset(const std::string& path);
	void Release(T* asset);

	size_t Size() const;
	const std::string& GetPath(T* asset) const;

protected:
	void Subscribe(T* asset);
	void Register(const std::string& path, T* asset);
	virtual T * Load(const std::string& path) = 0;
	virtual void Unload(T* asset) = 0;

private:
	typename std::map<std::string, T*>::const_iterator FindAssetIteratorByReference(T * asset) const;

private:
	std::map<std::string, T*> assets;
	std::map<T*,unsigned int> assetUsage;
};

#endif