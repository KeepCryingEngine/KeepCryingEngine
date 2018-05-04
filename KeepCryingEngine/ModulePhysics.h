#ifndef _MODULEPHYSICS_H_
#define _MODULEPHYSICS_H_

#include "Module.h"

#include <LinearMath\btTransform.h>
#include <list>

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
	~ModulePhysics();

	bool Init()override;
	bool Start()override;
	bool CleanUp()override;
	update_status PreUpdate()override;
	update_status Update()override;

	virtual void Play()override;
	virtual void Stop()override;

	void Subscribe(RigidBody & body);
	void Unsubscribe(RigidBody & body);

	btRigidBody* AddBody(btVector3 box_size, RigidBody* component);
private:
	void BuildBody(RigidBody & body);
	void DestroyBody(btRigidBody &body);

private:
	btDefaultCollisionConfiguration * collision_conf = nullptr;
	btCollisionDispatcher* dispatcher = nullptr;
	btBroadphaseInterface* broad_phase = nullptr;
	btSequentialImpulseConstraintSolver* solver = nullptr;
	btDiscreteDynamicsWorld* world = nullptr;

	DebugDraw* debugDraw = nullptr;

	std::list<RigidBody*> bodies;
};

#endif // !_MODULEPHYSICS_H_

