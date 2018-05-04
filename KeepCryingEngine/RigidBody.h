#ifndef _RIGIDBODY_H_
#define _RIGIDBODY_H_

#include "Component.h"

#include <LinearMath\btMotionState.h>
#include <LinearMath\btTransform.h>

class btRigidBody;

enum class BodyType
{
	SPHERE,
	BOX,
	CAPSULE
};

class RigidBody : public Component,public btMotionState
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
	const float3& GetBoxShape() const;

	// from btMotionState
	void getWorldTransform(btTransform& worldTrans) const override;
	void setWorldTransform(const btTransform& worldTrans) override;
private:
	BodyType bodyType = BodyType::SPHERE;

	float3 boxShape= float3::one;

	btRigidBody* body = nullptr;
};

#endif // !_RIGIDBODY_H_

