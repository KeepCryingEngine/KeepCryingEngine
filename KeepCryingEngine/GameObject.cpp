#include "GameObject.h"

#include <algorithm>

using namespace std;

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

void GameObject::SetParent(GameObject & newParent)
{
	vector<GameObject*>::iterator childParentIterator = find(parent->childs.begin(), parent->childs.end(), this);
	assert(childParentIterator != parent->childs.end());
	parent->childs.erase(childParentIterator);
	
	parent = &newParent;
	newParent.childs.push_back(this);
}

void GameObject::AddChild(GameObject & newChild)
{
	newChild.SetParent(*this);
}

Component & GameObject::AddComponent(ComponentType type)
{
	Component* component = nullptr;
	//component = ComponentFabric::CreateComponent(type);
	assert(component);
	components.push_back(component);
	return *component;
}

Component* GameObject::GetComponent(ComponentType type)
{
	for(Component* c : components)
	{
		if(c->type == type)
		{
			return c;
		}
	}
	return nullptr;
}

vector<Component*> GameObject::GetComponents(ComponentType type)
{
	vector<Component*> ret;
	for (Component* c : components) 
	{
		if (c->type == type) 
		{
			ret.push_back(c);
		}
	}
	return ret;
}