#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <list>
#include <vector>

#include <AABB.h>

#include "Component.h"
#include "ENGINE_UUID.h"


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
	void Update();
	// void LateUpdate() {}
	// void OnDisable() {}
	void OnDestroy();

	void SetAABB(const AABB& newAABB);
	AABB &GetAABB();

	bool GetVisible() const;
	void SetVisible(bool visible);

	void CheckIfFocuseableUI();
	void SetFocuseableUI(bool value);
	bool IsFocuseableUI()const;

	void CheckIfHovereableUI();
	void SetHovereableUI(bool value);
	bool IsHovereableUI()const;

	const ENGINE_UUID& UUID() const;

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
	ENGINE_UUID uuid;
	GameObject* parent = nullptr;

	std::list<Component*> toStart;
	std::list<Component*> toDestroy;

	std::vector<GameObject*> children;
	std::vector<Component*> components;
	Transform* transform = nullptr;
	std::string name;

	bool isFocuseableUI = false;
	bool isHovereableUI = false;
	bool enable = true;
	bool isStatic = false;
	int id;

	AABB aabb;

	bool visible = false;
};

template<typename T>
inline T * GameObject::AddComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	return (T*)AddComponent(T::TYPE);
}

template<typename T>
inline T * GameObject::GetComponent() const
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	return (T*)GetComponent(T::TYPE);
}

template <typename T>
std::vector<T*> GameObject::GetComponents() const
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	std::vector<Component*> components = GetComponents(T::TYPE);
	std::vector<T*> tComponents;
	std::transform(components.begin(), components.end(), std::back_inserter(tComponents), [](Component* c) -> T* { return (T*)c; });

	return tComponents;
}

template<typename T>
inline std::vector<T*> GameObject::GetComponentsInChildren() const
{
	static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

	std::vector<Component*> components = GetComponentsInChildren(T::TYPE);
	std::vector<T*> tComponents;
	std::transform(components.begin(), components.end(), std::back_inserter(tComponents), [](Component* c) -> T* { return (T*)c; });

	return tComponents;
}

#endif