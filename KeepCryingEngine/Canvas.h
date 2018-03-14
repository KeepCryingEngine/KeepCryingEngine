#ifndef _CANVAS_H_
#define _CANVAS_H_

#include <vector>

#include "Component.h"

class Canvas : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Canvas;

	Canvas();
	~Canvas();

	std::vector<Component::Type> GetNeededComponents() const override;
	void DrawUI() override;
};

#endif // !_CANVAS_H_

