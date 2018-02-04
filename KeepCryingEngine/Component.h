#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <imgui.h>

class GameObject;

enum class ComponentType
{
	Transform,
	Mesh,
	Material
};

class Component
{
public:
	Component(ComponentType type) : enabled(true),type(type) {}
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
	virtual void RealUpdate(float deltaTimeS, float realDeltaTimeS){};
	virtual void DrawUI(){};

public:
	bool enabled;
	ComponentType type;
	GameObject* gameObject;
};

#endif // !_COMPONENT_H_