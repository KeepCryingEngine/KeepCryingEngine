#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component.h"

#include <float3.h>
#include <float4x4.h>
#include <Quat.h>

class Transform : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Transform;

	Transform();
	virtual ~Transform();

	void DrawUI() override;

	virtual std::vector<Component::Type> GetProhibitedComponents() const override;

	void RealUpdate() override;

	float4x4 GetLocalMatrix() const;
	const float3& GetLocalPosition() const;
	const Quat& GetLocalRotation() const;
	const float3& GetEulerLocalRotation() const;
	const float3& GetLocalScale() const;

	void SetLocalModelMatrix(const float4x4& localMatrix);
	void SetLocalPosition(const float3& position);
	void SetLocalRotation(const Quat& rotation);
	void SetLocalScale(const float3& scale);
	void SetLocalTRS(const float3& translation, const Quat& rotation, const float3& scale);

	const float4x4& GetModelMatrix() const;
	const float3& GetWorldPosition() const;
	const Quat& GetWorldRotation() const;
	const float3& GetWorldScale() const;

	void SetModelMatrix(const float4x4& matrix);
	void SetWorldPosition(const float3& position);
	void SetWorldRotation(const Quat& rotation);
	void SetWorldScale(const float3& scale);
	void SetWorldTRS(const float3& translation, const Quat& rotation, const float3& scale);

	void Recalculate();

	const float3& Up() const;
	const float3& Forward() const;
	const float3& Right() const;

	const float3& Velocity() const;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Load(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	void Decompose(const float4x4& matrix, float3& position, Quat& rotation, float3& scale) const;

	void UpdateVelocity();

	// Const method that modify mutable data
	void SetDirty() const; 
	const float4x4& GetParentMatrix() const;
	void RecalculateIfNecessary() const;
	float4x4 CalculateModelMatrix() const;
	float3 CalculateWorldPosition() const;
	Quat CalculateWorldRotation() const;
	float3 CalculateWorldScale() const;

	float4x4 ParentModelMatrix() const;
	float3 ParentWorldPosition() const;
	Quat ParentWorldRotation() const;
	float3 ParentWorldScale() const;

private:
	float3 localPosition;
	Quat localRotation;
	float3 localScale;

	float3 velocity;
	mutable float3 previousWorldPosition;

	mutable bool dirty;
	mutable float4x4 modelMatrix;
	mutable float3 worldPosition;
	mutable Quat worldRotation;
	mutable float3 worldScale;
	mutable float3 eulerLocalRotation;

	mutable float3 up;
	mutable float3 forward;
	mutable float3 right;

};

#endif // !_TRANSFORM_H_