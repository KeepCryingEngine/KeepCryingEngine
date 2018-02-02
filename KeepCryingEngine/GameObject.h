#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <vector>
#include <assert.h>

class Component;

class GameObject
{
public:
	GameObject();
	~GameObject();

	GameObject& GetParent() {
		assert(parent);
		return *parent;
	}

	GameObject& GetChild(int child) {
		assert(childs.size < child);
		return *childs[child];
	}

	void SetParent(GameObject& newParent);

private:
	GameObject * parent;
	std::vector<GameObject*> childs;

	std::vector<Component*> components;
};

#endif