#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Component.h"

class Image : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Image;

	Image();
	~Image();
};

#endif // !_IMAGE_H_

