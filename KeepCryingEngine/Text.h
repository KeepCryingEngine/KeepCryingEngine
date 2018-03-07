#ifndef _TEXT_H_
#define _TEXT_H_

#include "Component.h"

class Text : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Text;

	Text();
	~Text();
};

#endif // !_TEXT_H_

