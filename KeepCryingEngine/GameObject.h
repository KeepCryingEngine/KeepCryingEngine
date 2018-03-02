#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <list>
#include <vector>

#include <AABB.h>

#include "Component.h"

class Transform;

class GameObject
{
public:
	GameObject(const std::string& name);
	virtual ~GameObject();

	GameObject* GetParent() const;
	void SetParent(GameObject& newParent);


	size_t ChildCount() const;

	GameObject* GetChild(size_t index) const;
	void RemoveChild(GameObject& child);

	const std::vector<GameObject*>& GetChildren() const;

	GameObject* GetById(unsigned long long int gameObjectId) const;
	void DeleteChild(GameObject& childToRemove);

	const std::string& GetName() const;

	int GetId() const;

	const bool IsEnabled() const;
	const bool IsStatic() const;

	void SetStatic(bool value);


	template <typename T>
	T* AddComponent();

	void RemoveComponent(Component* component);

	template <typename T>
	T* GetComponent() const;

	const std::vector<Component*>& GetComponents() const;

	template <typename T>
	std::vector<T*> GetComponents() const;

	template <typename T>
	std::vector<T*> GetComponentsInChildren() const;

	Transform* GetTransform() const;

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
	AABB &GetAABB();

	bool GetVisible() const;
	void SetVisible(bool visible);

private:
	Component * AddComponent(Component::Type type);

	Component* GetComponent(Component::Type type) const;
	std::vector<Component*> GetComponents(Component::Type type) const;
	std::vector<Component*> GetComponentsInChildren(Component::Type type) const;

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
	Transform* transform = nullptr;
	std::string name;

	bool enable = true;
	bool isStatic = false;
	int id;

	AABB aabb;

	bool visible = false;
};

template<typename T>
inline T * GameObject::AddComponent()
{
	return (T*)AddComponent(T::TYPE);
}

template<typename T>
inline T * GameObject::GetComponent() const
{
	return (T*)GetComponent(T::TYPE);
}

template <typename T>
std::vector<T*> GameObject::GetComponents() const
{
	return (std::vector<T*>)GetComponents(T::TYPE);
}

template<typename T>
inline std::vector<T*> GameObject::GetComponentsInChildren() const
{
	/* std::vector<T*> tComponents;

	std::vector<Component*> components = GetComponentsInChildren(T::TYPE);

	tComponents.insert(tComponents.end(), components.begin(), components.end());

	return tComponents; */

	return (std::vector<T*>)GetComponentsInChildren(T::TYPE);
}

#endif