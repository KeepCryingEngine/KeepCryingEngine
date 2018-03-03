#include "ComponentFabric.h"

#include <assert.h>

#include "Transform.h"
#include "Camera.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "AudioListener.h"
#include "AudioSource.h"

Component * ComponentFabric::CreateComponent(Component::Type type)
{
	Component* component = nullptr;
	switch (type)
	{
		case Component::Type::Transform:
			component = new Transform();
			break;
		case Component::Type::Camera:
			component = new Camera();
			break;
		case Component::Type::MeshFilter:
			component = new MeshFilter();
			break;
		case Component::Type::MeshRenderer:
			component = new MeshRenderer();
			break;
		case Component::Type::Animator:
			component = new Animator();
			break;
		case Component::Type::AudioListener:
			component = new AudioListener();
			break;
		case Component::Type::AudioSource:
			component = new AudioSource();
			break;
	}
	assert(component);
	return component;
}

ComponentFabric::ComponentFabric()
{
}
