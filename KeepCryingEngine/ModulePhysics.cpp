#include "ModulePhysics.h"

#include <btBulletDynamicsCommon.h>

#include <algorithm>

#include "Application.h"
#include "ModuleTime.h"
#include "ModuleEditorUI.h"
#include "DebugDraw.h"
#include "RigidBody.h"
#include "Transform.h"
#include "ModuleCamera.h"

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
	world->setGravity(btVector3(0.0f, gravity, 0.0f));

	return true;
}

bool ModulePhysics::CleanUp()
{
	for(btRigidBody*& btBody : btBodies)
	{
		delete(btBody);
		btBody = nullptr;
	}
	
	btBodies.clear();

	for(RigidBody* body : bodies)
	{
		body->SetBody(nullptr);
	}

	bodies.clear();

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	world->stepSimulation(App->time->GetDeltaTime(), 15);

	for(RigidBody* body : bodies)
	{
		UpdateTransform(body);
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModulePhysics::Update()
{
	//if(App->uiEditor->GetDebugMode())
	{
		if(App->camera->GetPlayOrEditorCamera() != nullptr)
		{
			world->debugDrawWorld();
		}
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	for(RigidBody* body : bodies)
	{
		UpdateBody(body);
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
			RemoveBodyFromWorld(*tempBody);
		}
	}

	CleanUp();
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
			RemoveBodyFromWorld(*tempBody);
		}
	}
}

void ModulePhysics::RebuildBody(RigidBody & body)
{	
	Unsubscribe(body);
	btRigidBody* tempBody = body.GetBody();
	btBodies.remove(tempBody);
	delete(tempBody);
	Subscribe(body);
}

void ModulePhysics::SetGravity(float value)
{
	gravity = value;
	world->setGravity(btVector3(0.0f, gravity, 0.0f));
}

float ModulePhysics::GetGravity() const
{
	return gravity;
}

btRigidBody* ModulePhysics::AddBody(RigidBody* component)
{
	btCollisionShape* colShape = component->CreateCollisionShape();

	float mass = component->GetMass(); // 0.0f would create a static or inmutable body
	btVector3 localInertia(0.f, 0.f, 0.f);
	if(mass != 0.f)
	{
		colShape->calculateLocalInertia(mass, localInertia);
	}
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,component,colShape,localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	btBodies.push_back(body);
	world->addRigidBody(body);

	UpdateBody(component);

	return body;
}

void ModulePhysics::RemoveBodyFromWorld(btRigidBody& body)
{
	// ...

	world->removeRigidBody(&body);

	// ...
}

void ModulePhysics::UpdateTransform(RigidBody* body) const
{
	float3 position = body->gameObject->GetTransform()->GetWorldPosition();
	Quat rotation = body->gameObject->GetTransform()->GetWorldRotation();

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(position.x, position.y, position.z));
	transform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));

	body->setWorldTransform(transform);
}

void ModulePhysics::UpdateBody(RigidBody* body) const
{
	if(body->GetBody() != nullptr)
	{
		btTransform transform;
		transform.setIdentity();
		body->getWorldTransform(transform);
		body->GetBody()->setWorldTransform(transform);
	}
}