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

	void ApplyConstraint(btRigidBody& ownBody);
	void ApplyConstraint(btRigidBody& ownBody, btRigidBody& secondBody);
};

#endif // !_HINGE_H_

