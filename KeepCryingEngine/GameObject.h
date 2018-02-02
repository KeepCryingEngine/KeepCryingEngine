#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <vector>
#include <assert.h>

#include "Component.h"

class Component;

class GameObject
{
public:
	enum class State{
		ToEnable,
		ToAwake,
		ToStart,
		ToUpdate,
		ToDisable,
		ToDestroy
	};

	GameObject();
	~GameObject();

	void Awake();
	void Start();
	void Update(float deltaTimeS, float realDeltaTimeS);

	std::string& GetName() {
		return name;
	}

	GameObject& GetParent() {
		assert(parent);
		return *parent;
	}

	GameObject& GetChild(unsigned int child) {
		assert(childs.size() < child);
		return *childs[child];
	}

	std::vector<GameObject*> GetChilds()
	{
		return childs;//Verify, maybe a copy, or a const reference
	}

	void SetParent(GameObject& newParent);
	void AddChild(GameObject& newChild);
	
	Component& GetComponent(ComponentType type);
	std::vector<Component*> GetComponents();


private:
	GameObject * parent;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	std::string name;
};

#endif