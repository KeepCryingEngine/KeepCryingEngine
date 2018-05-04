#include "ModulePhysics.h"

#include <btBulletDynamicsCommon.h>

#include <algorithm>

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleEditorUI.h"
#include "DebugDraw.h"
#include "RigidBody.h"

ModulePhysics::ModulePhysics()
{ }

ModulePhysics::~ModulePhysics()
{ }

bool ModulePhysics::Init()
{
	collisionConf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConf);
	broadPhase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();

	debugDraw = new DebugDraw();

	return true;
}

bool ModulePhysics::Start()
{
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, solver, collisionConf);
	world->setDebugDrawer(debugDraw);
	world->setGravity(btVector3(0.0f, -10.0f, 0.0f));

	return true;
}

bool ModulePhysics::CleanUp()
{
	return true;
}

update_status ModulePhysics::PreUpdate()
{
	// Step the physics world
	world->stepSimulation(App->time->GetDeltaTime(), 15);
	return update_status::UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	//if(App->uiEditor->GetDebugMode())
	{
		world->debugDrawWorld();
	}

	return update_status::UPDATE_CONTINUE;
}

void ModulePhysics::Play()
{
	for(RigidBody* body : bodies)
	{
		body->SetBody(AddBody(body));
	}
}

void ModulePhysics::Stop()
{
	for(RigidBody* body : bodies)
	{
		btRigidBody* tempBody = body->GetBody();
		if(tempBody != nullptr)
		{
			DestroyBody(*tempBody);
		}
	}
}

void ModulePhysics::Subscribe(RigidBody& body)
{
	if(bodies.insert(&body).second)
	{
		if(App->state == TimeState::PLAYING)
		{
			body.SetBody(AddBody(&body));
		}
	}
}

void ModulePhysics::Unsubscribe(RigidBody& body)
{
	if(bodies.erase(&body) > 0)
	{
		btRigidBody* tempBody = body.GetBody();

		if(tempBody != nullptr)
		{
			DestroyBody(*tempBody);
		}
	}
}

btRigidBody* ModulePhysics::AddBody(RigidBody* component)
{
	btCollisionShape* colShape = nullptr;
	switch(component->GetBodyType())
	{
		case BodyType::SPHERE:
			break;
		case BodyType::BOX:
		{
			float3 boxSize = component->GetBoxShape();
			btVector3 tempVec = btVector3(boxSize.x, boxSize.y, boxSize.z);
			colShape = new btBoxShape(tempVec); // regular box
		}
			break;
		case BodyType::CAPSULE:
			break;
		default:
			break;
	}

	float mass = component->GetMass(); // 0.0f would create a static or inmutable body
	btVector3 localInertia(0.f, 0.f, 0.f);
	if(mass != 0.f)
	{
		colShape->calculateLocalInertia(mass, localInertia);
	}
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,component,colShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	world->addRigidBody(body);
	return body;
}

void ModulePhysics::DestroyBody(btRigidBody& body)
{
	// ...

	world->removeRigidBody(&body);

	// ...
}