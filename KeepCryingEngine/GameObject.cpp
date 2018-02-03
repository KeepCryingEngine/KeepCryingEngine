#include "GameObject.h"

#include "Globals.h"
#include "ComponentFabric.h"

using namespace std;

GameObject::GameObject()
{ }

GameObject::~GameObject()
{ }

GameObject* GameObject::GetParent() const
{
	assert(parent);

	return parent;
}

size_t GameObject::GetChildCount() const
{
	return children.size();
}

GameObject* GameObject::GetChild(size_t index) const
{
	assert(children.size() > index);

	return children[index];
}

const std::vector<GameObject*>& GameObject::GetChildren() const
{
	return children;
}

void GameObject::AddChild(GameObject& newChild)
{
	newChild.SetParent(*this);
}

void GameObject::SetParent(GameObject & newParent)
{
	vector<GameObject*>::iterator childParentIterator = find(parent->children.begin(), parent->children.end(), this);
	assert(childParentIterator != parent->children.end());
	parent->children.erase(childParentIterator);
	
	parent = &newParent;
	newParent.children.push_back(this);
}

void GameObject::Update(float deltaTimeS, float realDeltaTimeS)
{
	CheckToStart();
	CheckToDestroy();

	for(Component* component : components)
	{
		/* if(component->IsEnabled())
		{
			component->Update(deltaTimeS, realDeltaTimeS);
		} */
	}
}

void GameObject::OnDestroy()
{
	for(Component* component : components)
	{
		// component->Destroy();
	}

	for(Component* component : components)
	{
		RELEASE(component);
	}

	components.clear();
}

Component* GameObject::AddComponent(ComponentType type)
{
	Component* component = ComponentFabric::CreateComponent(type);
	/*

	component->SetGameObject(this);
	component->Awake();

	toStart.push_back(component);

	*/
	assert(component);
	return component;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for(Component* component : components)
	{
		if(component->type == type)
		{
			return component;
		}
	}

	return nullptr;
}

std::vector<Component*> GameObject::GetComponents(ComponentType type)
{
	std::vector<Component*> ret;

	for (Component* component : components)
	{
		if (component->type == type)
		{
			ret.push_back(component);
		}
	}

	return ret;
}

void GameObject::GetComponents(ComponentType type, std::vector<Component*>& ret)
{
	for (Component* component : components)
	{
		if (component->type == type)
		{
			ret.push_back(component);
		}
	}
}

void GameObject::CheckToStart()
{
	for(Component* component : toStart)
	{
		// component->Start();

		components.push_back(component);
	}

	toStart.clear();
}

void GameObject::CheckToDestroy()
{
	for(Component* component : toDestroy)
	{
		components.erase(find(components.begin(), components.end(), component));

		// component->Destroy();
	}

	for(Component* component : toDestroy)
	{
		RELEASE(component);
	}

	/* for(Component* component : toDestroy)
	{
		components.erase(find(components.begin(), components.end(), component));

		DestroyAndRelease(component);
	} */

	toDestroy.clear();
}

/* void GameObject::DestroyAndRelease(Component* component) const
{
	// component->Destroy();

	RELEASE(component);
} */