#ifndef _COMPONENTFABRIC_H_
#define _COMPONENTFABRIC_H_

#include "Component.h"

class ComponentFabric
{
public:
	static Component* CreateComponent(Component::Type type);

private:
	ComponentFabric();
};

#endif
