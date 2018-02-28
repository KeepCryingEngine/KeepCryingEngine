#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <vector>
#include <imgui.h>

class GameObject;

class Component
{
public:
	enum class Type
	{
		Transform,
		Camera,
		MeshFilter,
		MeshRenderer,
		Animator,
		AudioListener,
		AudioSource
	};

	Component(Component::Type type) : enabled(true), wasEnabled(true), type(type)
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

	virtual std::vector<Component::Type> GetNeededComponents() const { return { }; }
	virtual std::vector<Component::Type> GetProhibitedComponents() const { return { }; }

public:
	bool enabled;
	bool wasEnabled;
	Component::Type type;
	GameObject* gameObject = nullptr;
};

#endif // !_COMPONENT_H_