#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"

#include <experimental/filesystem>

class Texture;

class Image : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Image;

	Image();
	virtual ~Image();

	void DrawUI() override;

	void SetTextureByPath(const std::experimental::filesystem::path& path);

	Texture * GetTexture() const;
private:
	Texture * texture = nullptr;
};

#endif // !_IMAGE_H_

