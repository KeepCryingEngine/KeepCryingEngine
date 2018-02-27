#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <vector>
#include <imgui.h>

class GameObject;

enum class ComponentType
{
	Transform,
	Camera,
	MeshFilter,
	MeshRenderer,
	Animator
};

class Component
{
public:
	Component(ComponentType type) : enabled(true), wasEnabled(true), type(type)
	{}
	virtual ~Component() {}

	virtual void Awake(){};
	virtual void Start(){};
	virtual void Destroy(){};

	virtual void Update(float deltaTimeS, float realDeltaTimeS)
	{
		if(enabled)
		{
			RealUpdate(deltaTimeS,realDeltaTimeS);
		}
	};

	virtual void SetEnable(bool setEnable)
	{
		enabled = setEnable;
	};

	virtual void RealUpdate(float deltaTimeS, float realDeltaTimeS){};
	
	virtual void DrawUI(){};

	virtual std::vector<ComponentType> GetNeededComponents() const { return { }; }
	virtual std::vector<ComponentType> GetProhibitedComponents() const { return { }; }

public:
	bool enabled;
	bool wasEnabled;
	ComponentType type;
	GameObject* gameObject = nullptr;
};

#endif // !_COMPONENT_H_