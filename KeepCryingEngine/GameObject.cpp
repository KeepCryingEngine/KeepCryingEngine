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
	

}
