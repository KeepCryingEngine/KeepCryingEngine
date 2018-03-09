#ifndef _CANVAS_H_
#define _CANVAS_H_

#include "Component.h"
#include "GameUI.h"

class Canvas : public Component, public GameUI
{
public:
	static const Component::Type TYPE = Component::Type::Canvas;

	Canvas();
	~Canvas();
};

#endif // !_CANVAS_H_

