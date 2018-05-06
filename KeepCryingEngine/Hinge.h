#ifndef _HINGE_H_
#define _HINGE_H_

#include "Component.h"
#include <btBulletDynamicsCommon.h>

class Hinge :public Component
{
public:
	static const Component::Type TYPE = Component::Type::Hinge;
	Hinge();
	~Hinge();

	void RealUpdate()override;

	virtual void DrawUI() override;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

	void ApplyConstraint(btRigidBody& ownBody);
	void ApplyConstraint(btRigidBody& ownBody, btRigidBody& secondBody);
private:
	btHingeConstraint * constraint = nullptr;

	float3 ownPivot = float3::zero;
	float3 ownAngle = float3::one;

	float3 secondPivot = float3::zero;
	float3 secondAngle = float3::one;

	bool usingSecond = false;
};

#endif // !_HINGE_H_

