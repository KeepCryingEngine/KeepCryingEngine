#ifndef _BALLSOCKET_H_
#define _BALLSOCKET_H_

#include "Component.h"

#include <btBulletDynamicsCommon.h>

class BallSocket :public Component
{
public:
	static const Component::Type TYPE = Component::Type::BallSocket;
	BallSocket();
	~BallSocket();

	virtual void RealUpdate() override;
	virtual void Destroy() override;

	virtual void DrawUI() override;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

	void ApplyConstraint(btRigidBody& ownBody);
	void ApplyConstraint(btRigidBody& ownBody, btRigidBody& secondBody);

	std::vector<Component::Type> GetProhibitedComponents() const override;
	std::vector<Component::Type> GetNeededComponents() const override;

private:
	float3 ownPivot = float3::zero;
	float3 secondPivot = float3::zero;

	bool usingSecond = false;

	btPoint2PointConstraint* constraint = nullptr;
};

#endif // !_BALLSOCKET_H_

