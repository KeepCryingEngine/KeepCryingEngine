#ifndef _ASSET_H_
#define _ASSET_H_

enum class AssetType {
	AudioClip,
	Mesh,
	Animation,
	Texture
};

template <typename K>
class Asset
{
public:
	Asset(const K& identifier, AssetType type);
	virtual ~Asset();

	const K& Identifier() const;
	AssetType Type() const;

private:
	AssetType type;
	K identifier;

};

template <typename K>
inline Asset<K>::Asset(const K& identifier, AssetType type) :
	identifier(identifier),
	type(type)
{
}

template <typename K>
inline Asset<K>::~Asset()
{
}

template<typename K>
inline const K & Asset<K>::Identifier() const
{
	return identifier;
}

template <typename K>
inline AssetType Asset<K>::Type() const
{
	return type;
}

#endif