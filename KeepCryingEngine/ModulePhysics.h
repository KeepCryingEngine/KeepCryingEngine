#ifndef _MODULEPHYSICS_H_
#define _MODULEPHYSICS_H_

#include "Module.h"

#include <set>
#include <list>
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
	update_status PostUpdate() override;

	virtual void Play() override;
	virtual void Stop() override;

	void Subscribe(RigidBody& body);
	void Unsubscribe(RigidBody& body);
	void RebuildBody(RigidBody& body);
	void SetBodyFlags(btRigidBody& body,bool isStatic, bool isKinematic);
	void SetBodyConstraints(btRigidBody& body,const float3& linearFactor, const float3& angularFactor);

	void SetGravity(float value);
	float GetGravity()const;
	btDiscreteDynamicsWorld* GetWorld() const;

private:
	btRigidBody* AddBody(RigidBody* component);
	void RemoveBodyFromWorld(btRigidBody& body);
	void UpdateTransform(RigidBody* body) const;
	void UpdateBody(RigidBody* body) const;

private:
	btDefaultCollisionConfiguration * collisionConf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broadPhase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld * world = nullptr;
	

	DebugDraw* debugDraw = nullptr;

	float gravity = -9.8f;

	std::set<RigidBody*> bodies;
	std::list<btRigidBody*> btBodies;
};

#endif // !_MODULEPHYSICS_H_