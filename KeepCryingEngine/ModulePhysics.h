#ifndef _MODULEPHYSICS_H_
#define _MODULEPHYSICS_H_

#include "Module.h"

#include <set>
#include <float3.h>

class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class RigidBody;
class btRigidBody;

class DebugDraw;

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	virtual ~ModulePhysics();

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;
	update_status PreUpdate() override;
	update_status Update() override;

	virtual void Play() override;
	virtual void Stop() override;

	void Subscribe(RigidBody& body);
	void Unsubscribe(RigidBody& body);

private:
	btRigidBody* AddBody(RigidBody* component);
	void DestroyBody(btRigidBody& body);

private:
	btDefaultCollisionConfiguration * collisionConf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	DebugDraw* debugDraw = nullptr;

	std::set<RigidBody*> bodies;
};

#endif // !_MODULEPHYSICS_H_