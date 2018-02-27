#include "ComponentFabric.h"

#include <assert.h>

#include "Transform.h"
#include "Camera.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Animator.h"

Component * ComponentFabric::CreateComponent(ComponentType type)
{
	Component* component = nullptr;
	switch (type)
	{
		case ComponentType::Transform:
			component = new Transform();
			break;
		/*case ComponentType::Material:
			component = new Material();
			break;*/
		/*case ComponentType::Mesh:
			component = new Mesh();
			break;*/
		case ComponentType::Camera:
			component = new Camera();
			break;
		case ComponentType::MeshFilter:
			component = new MeshFilter();
			break;
		case ComponentType::MeshRenderer:
			component = new MeshRenderer();
			break;
		case ComponentType::Animator:
			component = new Animator();
			break;
	}
	assert(component);
	return component;
}

ComponentFabric::ComponentFabric()
{
}
