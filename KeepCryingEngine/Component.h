#ifndef _COMPONENT_H_
#define _COMPONENT_H_

enum class ComponentType
{
	Transform,
	Mesh,
	Material
};

class Component
{
public:
	Component(ComponentType type) : enabled(true),type(type) {}
	virtual ~Component() {}

public:
	bool enabled;
	ComponentType type;
};

#endif // !_COMPONENT_H_