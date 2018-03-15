#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <vector>
#include <imgui.h>
#include <json.hpp>
#include "ENGINE_UUID.h"

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
		AudioSource,
		ReverbZone,
		Canvas,
		Image,
		Button,
		Text,
		InpuText,
		Transform2D
	};

	Component(Component::Type type) : enabled(true), wasEnabled(true), type(type)
	{}
	virtual ~Component() {}

	virtual void Awake(){};
	virtual void Start(){};
	virtual void Destroy(){};

	virtual void Update()
	{
		if(enabled)
		{
			RealUpdate();
		}
	};

	virtual void SetEnable(bool setEnable)
	{
		enabled = setEnable;
	};

	virtual void RealUpdate(){};
	
	virtual void DrawUI(){};

	virtual std::vector<Component::Type> GetNeededComponents() const { return { }; }
	virtual std::vector<Component::Type> GetProhibitedComponents() const { return { }; }

	virtual bool IsFocuseableUI() const
	{
		return isFocuseableUI;
	}

	virtual bool IsHovereableUI() const
	{
		return isHovereableUI;
	}

	virtual void PreLoad(const nlohmann::json& json) = 0;
	virtual void Load(const nlohmann::json& json) = 0;
	virtual void Save(nlohmann::json& json) const = 0;

public:
	ENGINE_UUID uuid;
	bool enabled;
	bool wasEnabled;
	Component::Type type;
	GameObject* gameObject = nullptr;
	bool isFocuseableUI = false;
	bool isHovereableUI = false;
};

#endif // !_COMPONENT_H_