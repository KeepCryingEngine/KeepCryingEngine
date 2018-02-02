#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <list>
#include <vector>
// #include <assert.h>

#include "Component.h"

class GameObject
{
public:
	/* enum class State{
		ToAwake,
		ToEnable,
		ToStart,
		ToUpdate,
		ToDisable,
		ToDestroy
	}; */

	GameObject();
	virtual ~GameObject();

	GameObject* GetParent() const;

	size_t GetChildCount() const;

	GameObject* GetChild(size_t index) const;
	const std::vector<GameObject*>& GetChildren() const;

	/* GameObject& GetParent() {
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
	} */

	void AddChild(GameObject& child);
	void SetParent(GameObject& newParent);

	template <class T>
	T* AddComponent();

	template <class T>
	T* GetComponent();

	template <class T>
	std::vector<T*> GetComponents();

	template <class T>
	void GetComponents(std::vector<T*>& components);

	// void Awake() {}
	// void OnEnable() {}
	// void Start() {}
	// void PreUpdate() {}
	void Update(float deltaTimeS, float realDeltaTimeS);
	// void LateUpdate() {}
	// void OnDisable() {}
	void OnDestroy();
	
private:
	void CheckToStart();
	void CheckToDestroy();

	// void DestroyAndRelease(Component* component) const;

private:
	// State state;

	GameObject* parent = nullptr;

	std::list<Component*> toStart;
	std::list<Component*> toDestroy;

	std::vector<GameObject*> children;
	std::vector<Component*> components;
};

#endif