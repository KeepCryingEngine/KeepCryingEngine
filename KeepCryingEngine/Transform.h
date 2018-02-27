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

	float4x4 GetLocalMatrix() const;
	const float3& GetLocalPosition() const;
	const Quat& GetLocalRotation() const;
	const float3& GetEulerLocalRotation() const;
	const float3& GetLocalScale() const;

	//void SetLocalMatrix(const float4x4& localMatrix);
	void SetLocalPosition(const float3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const float3& scale);
	void SetLocalTRS(const float3& translation, const Quat& rotation, const float3& scale);

	const float4x4& GetModelMatrix() const;
	const float3& GetWorldPosition() const;
	const Quat& GetWorldRotation() const;
	const float3& GetWorldScale() const;

	//void SetModelMatrix(const float4x4& matrix);
	void SetWorldPosition(const float3& position);
	void SetWorldRotation(const Quat& rotation);
	void SetWorldScale(const float3& scale);
	void SetWorldTRS(const float3& translation, const Quat& rotation, const float3& scale);


	void GuizmoSetModelMatrix(const float4x4& modelMatrix, const float3& position, const float3& rotation, const float3& scale);

	void Recalculate();

private:
	// Const method that modify mutable data
	void SetDirty() const; 
	const float4x4& GetParentMatrix() const;
	void RecalculateIfNecessary() const;
	float4x4 CalculateModelMatrix() const;
	float3 CalculateWorldPosition() const;
	Quat CalculateWorldRotation() const;
	float3 CalculateWorldScale() const;

private:
	float3 localPosition;
	Quat localRotation;
	float3 localScale;

	mutable bool dirty;
	mutable float4x4 modelMatrix;
	mutable float3 worldPosition = { 0.0f, 0.0f, 0.0f };
	mutable Quat worldRotation;
	mutable float3 worldScale;
	mutable float3 eulerLocalRotation;

};

#endif // !_TRANSFORM_H_