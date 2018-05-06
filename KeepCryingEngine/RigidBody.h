#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "Component.h"

#include <LinearMath\btMotionState.h>

class btRigidBody;
class btTransform;
class btCollisionShape;

enum class BodyType
{
	BOX,
	SPHERE,
	CAPSULE
};

class RigidBody : public Component, public btMotionState
{
public:
	static const Component::Type TYPE = Component::Type::RigidBody;

	RigidBody();
	virtual ~RigidBody();

	virtual void Awake()override;
	virtual void Destroy()override;

	virtual void DrawUI()override;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

	void SetBodyType(BodyType newType);
	void SetBody(btRigidBody* newBody);
	BodyType GetBodyType() const;
	btRigidBody* GetBody()const;
	float GetMass()const;
	bool GetKinematic() const;
	const float3& GetLinearFactor() const;
	const float3& GetAngularFactor() const;

	// from btMotionState
	void getWorldTransform(btTransform& worldTrans) const override;
	void setWorldTransform(const btTransform& worldTrans) override;

	btCollisionShape* CreateCollisionShape() const;

private:
	struct BaseShapeInfo { };

	struct BoxShapeInfo : public BaseShapeInfo { float3 dimension = float3::one; };

	struct SphereShapeInfo : public BaseShapeInfo { float radius = 1.0f; };

	struct CapsuleShapeInfo : public BaseShapeInfo { float radius = 1.0f; float height = 2.0f; };

private:
	BodyType bodyType = BodyType::BOX;

	BaseShapeInfo* shapeInfo = nullptr;

	float mass = 1.0f;
	bool kinematic = false;
	float3 linearFactor = float3::one;
	float3 angularFactor = float3::one;

	btRigidBody* body = nullptr;
};

#endif // !_RIGIDBODY_H_