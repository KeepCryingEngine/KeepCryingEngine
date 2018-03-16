#ifndef _ASSETMANAGER_H_
#define _ASSETMANAGER_H_

#include <map>
#include <experimental/filesystem>


#include "Module.h"

template <typename K, typename T>
class AssetManager : public Module
{
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

#endif