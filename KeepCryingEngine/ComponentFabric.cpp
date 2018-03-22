#include "ComponentFabric.h"

#include <assert.h>

#include "Application.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Animator.h"
#include "AudioListener.h"
#include "AudioSource.h"
#include "ReverbZone.h"
#include "ModuleGameUI.h"
#include "Transform2D.h"
#include "Canvas.h"
#include "Image.h"
#include "Button.h"
#include "Text.h"
#include "InputText.h"
#include "GridRenderer.h"
#include "ParticleSystem.h"

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
		case Component::Type::ReverbZone:
			component = new ReverbZone();
			break;
		case Component::Type::Canvas:
			if(App->uiGame->GetCanvas() == nullptr)
			{
				component = new Canvas();
				App->uiGame->SetCanvas(*(Canvas*)component);
			}
			break;
		case Component::Type::Transform2D:
			component = new Transform2D();
			break;
		case Component::Type::Image:
			component = new Image();
			break;
		case Component::Type::Button:
			component = new Button();
			break;
		case Component::Type::Text:
			component = new Text();
			break;
		case Component::Type::InpuText:
			component = new InputText();
			break;
		case Component::Type::GridRenderer:
			component = new GridRenderer();
			break;
		case Component::Type::ParticleSystem:
			component = new ParticleSystem();
			break;
	}
	assert(component);
	return component;
}

ComponentFabric::ComponentFabric()
{
}
