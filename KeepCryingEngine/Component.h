#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class Component
{
public:
	Component() : enabled(true) {}
	virtual ~Component() {}

private:
	bool enabled;
};

#endif // !_COMPONENT_H_