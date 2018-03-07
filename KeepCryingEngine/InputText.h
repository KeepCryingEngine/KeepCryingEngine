#ifndef _INPUTTEXT_H_
#define _INPUTTEXT_H_

#include "Component.h"

class InputText : public Component
{
public:
	static const Component::Type TYPE = Component::Type::InpuText;

	InputText();
	~InputText();
};

#endif // !_INPUTTEXT_H_

