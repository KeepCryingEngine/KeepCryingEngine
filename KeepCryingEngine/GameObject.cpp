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

Component & GameObject::GetComponent()
{
	// TODO: insertar una instrucción return aquí
}

vector<Component&> GameObject::GetComponents()
{
	return vector<Component&>();
}
