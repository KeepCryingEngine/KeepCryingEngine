#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"

#include <float3.h>
#include <float4x4.h>
#include <Quat.h>

class Transform : public Component
{
public:
	Transform();
	virtual ~Transform();

	void DrawUI() override;

	virtual std::vector<ComponentType> GetProhibitedComponents() const override;

	const float3& GetLocalPosition() const;
	const Quat& GetLocalRotation() const;
	const float3& GetLocalScale() const;

	void SetLocalPosition(const float3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const float3& scale);

	const float3& GetWorldPosition() const;
	const Quat& GetWorldRotation() const;
	const float3& GetWorldScale() const;

	void SetWorldPosition(const float3& position);
	void SetWorldRotation(const Quat& rotation);
	void SetWorldScale(const float3& scale);

	const float4x4& GetModelMatrix() const;

private:
	void SetDirty() const;
	float4x4 GetLocalMatrix() const;
	const float4x4& GetParentMatrix() const;
	void RecalculateIfNecessary() const;

private:
	float3 localPosition;
	Quat localRotation;
	float3 localScale;

	mutable bool dirty;
	mutable float4x4 modelMatrix;
	mutable float3 worldPosition;
	mutable Quat worldRotation;
	mutable float3 worldScale;

	mutable float3 eulerLocalRotation;
};

#endif // !_TRANSFORM_H_