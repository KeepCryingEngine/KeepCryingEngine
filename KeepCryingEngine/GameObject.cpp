#include "GameObject.h"

#include <algorithm>

using namespace std;

GameObject::GameObject()
{
}


GameObject::~GameObject()
{
}

void GameObject::Awake()
{

}

void GameObject::Start()
{

}

void GameObject::Update(float deltaTimeS, float realDeltaTimeS)
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

Component & GameObject::GetComponent(ComponentType type)
{
	for each (Component* c in components)
	{
		if(c->type == type)
		{
			return *c;
		}
	}
}

vector<Component*> GameObject::GetComponents()
{
	return components;
}
