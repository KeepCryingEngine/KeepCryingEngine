#include "ComponentFabric.h"

#include <assert.h>

#include "Transform.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"

Component * ComponentFabric::CreateComponent(ComponentType type)
{
	Component* component = nullptr;
	switch (type)
	{
		case ComponentType::Transform:
			component = new Transform();
			break;
		case ComponentType::Material:
			component = new Material();
			break;
		case ComponentType::Mesh:
			component = new Mesh();
			break;
		case ComponentType::Camera:
			component = new Camera();
			break;
	}
	assert(component);
	return component;
}

ComponentFabric::ComponentFabric()
{
}
