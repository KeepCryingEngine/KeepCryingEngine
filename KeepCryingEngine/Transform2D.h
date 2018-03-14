#ifndef _TRANSFORM2D_H_
#define _TRANSFORM2D_H_

#include "Component.h"

#include <MathGeoLib.h>

class Transform2D : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Transform2D;

	Transform2D();
	virtual ~Transform2D();

	void DrawUI() override;

	float3 GetWorldPosition() const;
	float3 GetLocalPosition() const;
	
	void SetLocalPosition(const float3& position);
	void SetWorldPosition(const float3& position);

	float2 GetSize() const;
	float3 GetMinPosition() const;
	float3 GetMaxPosition() const;

	virtual void Load(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	float3 GetParentWorldPosition() const;

private:
	float3 localPosition;
	float2 size;
	float2 anchor;
	float2 pivot;

	bool cutChildren = false;
	bool isLocked = false;

};

#endif