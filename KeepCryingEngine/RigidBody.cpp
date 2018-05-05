#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModulePhysics.h"
#include <btBulletDynamicsCommon.h>

RigidBody::RigidBody():Component(TYPE)
{ }

RigidBody::~RigidBody()
{ }

void RigidBody::Awake()
{
	App->physics->Subscribe(*this);
}

void RigidBody::Destroy()
{
	App->physics->Unsubscribe(*this);
}

void RigidBody::DrawUI()
{
	if(ImGui::CollapsingHeader("RigidBody"))
	{
		if(ImGui::Checkbox("Active", &enabled))
		{
			if(enabled)
			{
				App->physics->Subscribe(*this);
			}
			else
			{
				App->physics->Unsubscribe(*this);
			}
		}

		ImGui::SameLine();

		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

		if(ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, 1000000.0f))
		{
			if(body != nullptr)
			{
				App->physics->Unsubscribe(*this);

				btVector3 inertia;
				body->getCollisionShape()->calculateLocalInertia(mass, inertia);
				body->setMassProps(mass, inertia);

				App->physics->Subscribe(*this);
			}
		}

		int tempType = (int)bodyType;
		if(ImGui::Combo("Body Type", &tempType, "Box\0Sphere\0Capsule"))
		{
			bodyType = (BodyType)tempType;
		}

		switch(bodyType)
		{
			case BodyType::BOX:
			{
				if(ImGui::DragFloat3("Size", boxShape.ptr(), 0.1f))
				{
					if(body != nullptr)
					{
						((btBoxShape*)body->getCollisionShape())->setImplicitShapeDimensions(btVector3(boxShape.x, boxShape.y, boxShape.z));

						body->applyForce(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));
					}
				}
			}
			break;
		}
	}
}

void RigidBody::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);
	bodyType = json["bodyType"];
	mass = json["mass"];
	from_json(json["boxShape"], boxShape);
}

void RigidBody::Save(nlohmann::json & json) const
{
	Component::Save(json);
	json["bodyType"] = bodyType;
	json["mass"] = mass;

	nlohmann::json jsonBoxShape;
	to_json(jsonBoxShape, boxShape);

	json["boxShape"] = jsonBoxShape;
}

void RigidBody::SetBodyType(BodyType newType)
{
	bodyType = newType;
}

void RigidBody::SetBody(btRigidBody * newBody)
{
	body = newBody;
}

BodyType RigidBody::GetBodyType() const
{
	return bodyType;
}

btRigidBody * RigidBody::GetBody() const
{
	return body;
}

const float3 & RigidBody::GetBoxShape() const
{
	return boxShape;
}

float RigidBody::GetMass() const
{
	return mass;
}

void RigidBody::getWorldTransform(btTransform & worldTrans) const
{
	float3 tempVec = gameObject->GetTransform()->GetWorldPosition();
	worldTrans.setOrigin(btVector3(tempVec.x,tempVec.y,tempVec.z));

	Quat tempQuat = gameObject->GetTransform()->GetLocalRotation();
	worldTrans.setRotation(btQuaternion(tempQuat.x,tempQuat.y,tempQuat.z,tempQuat.w));
}

void RigidBody::setWorldTransform(const btTransform & worldTrans)
{
	btQuaternion rot = worldTrans.getRotation();
	btVector3 pos = worldTrans.getOrigin();

	Quat tempQuat(rot.getX(), rot.getY(), rot.getZ(), rot.getW());
	float3 tempVec(pos.getX(), pos.getY(), pos.getZ());

	float4x4 new_global(tempQuat, tempVec);

	// now find out our new local transformation in order to meet the global one from physics
	float4x4 new_local = new_global * gameObject->GetParent()->GetTransform()->GetLocalMatrix().Inverted();
	float3 translation, scale;
	Quat rotation;

	new_local.Decompose(translation, rotation, scale);
	gameObject->GetTransform()->SetLocalPosition(translation);
	gameObject->GetTransform()->SetLocalRotation(rotation);
	gameObject->GetTransform()->SetLocalScale(scale);
}