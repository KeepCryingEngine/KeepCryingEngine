#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <list>
#include <vector>

#include <AABB.h>

#include "Component.h"

class GameObject
{
public:
	GameObject(const std::string& name);
	virtual ~GameObject();

	GameObject* GetParent() const;

	size_t GetChildCount() const;

	GameObject* GetChild(size_t index) const;
	const std::vector<GameObject*>& GetChildren() const;

	GameObject* GetChild(unsigned long long int gameObjectId) const;
	GameObject* GetSelfOrChild(unsigned long long int gameObjectId) const;
	void DeleteChildFromList(GameObject& childToRemove);

	const std::string& GetName() const;

	unsigned long long int GetId() const;

	const bool IsEnabled() const;

	void SetParent(GameObject& newParent);

	Component* AddComponent(ComponentType type, bool forceAddition = false);
	void RemoveComponent(Component* component);
	Component* GetComponent(ComponentType type) const;
	const std::vector<Component*>& GetComponents() const;
	std::vector<Component*> GetComponents(ComponentType type);
	void GetComponents(ComponentType type, std::vector<Component*>& components);

	void DrawUI();

	// void Awake() {}
	// void OnEnable() {}
	// void Start() {}
	// void PreUpdate() {}
	void Update(float deltaTimeS, float realDeltaTimeS);
	// void LateUpdate() {}
	// void OnDisable() {}
	void OnDestroy();

	void SetAABB(const AABB& newAABB);

private:
	void CheckToStart();
	void CheckToDestroy();

	void AddInternalComponent(Component* component);

	bool CanAttach(const Component& component) const;

private:
	GameObject* parent = nullptr;

	std::list<Component*> toStart;
	std::list<Component*> toDestroy;

	std::vector<GameObject*> children;
	std::vector<Component*> components;
	std::string name;

	bool enable = true;
	unsigned long long int id;

	AABB aabb;
};

#endif