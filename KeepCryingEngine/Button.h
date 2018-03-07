#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "Component.h"

class Button : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Button;

	Button();
	~Button();

};

#endif // !_BUTTON_H_

