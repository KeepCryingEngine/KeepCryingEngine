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
		ToAwake,
		ToEnable,
		ToStart,
		ToUpdate,
		ToDisable,
		ToDestroy
	};

	GameObject();
	virtual ~GameObject();

	GameObject& GetParent() {
		assert(parent);
		return *parent;
	}

	size_t ChildCount() const {
		return childs.size();
	}

	GameObject& GetChild(size_t child) {
		assert(childs.size() < child);
		return *childs[child];
	}

	const std::vector<GameObject*>& GetChilds()
	{
		return childs;
	}

	void SetParent(GameObject& newParent);
	void AddChild(GameObject& newChild);
	
	Component& AddComponent(ComponentType);
	Component* GetComponent(ComponentType);
	std::vector<Component*> GetComponents(ComponentType);

	virtual void Awake() {}
	virtual void OnEnable() {}
	virtual void Start() {}
	virtual void PreUpdate() {}
	virtual void Update(float deltaTimeS, float realDeltaTimeS) {}
	virtual void LateUpdate() {}
	virtual void OnDisable() {}
	virtual void OnDestroy() {}
	
private:
	State state;

	GameObject * parent;
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
};

#endif