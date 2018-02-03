#ifndef _COMPONENTFABRIC_H_
#define _COMPONENTFABRIC_H_

#include "Component.h"

class ComponentFabric
{
public:
	static Component* CreateComponent(ComponentType type);

private:
	ComponentFabric();
};

#endif
