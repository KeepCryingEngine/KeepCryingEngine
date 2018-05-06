#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModulePhysics.h"
#include <btBulletDynamicsCommon.h>

RigidBody::RigidBody() : Component(TYPE)
{
	SetBodyType(bodyType);
}

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

		if(ImGui::Checkbox("Kinematic",&kinematic))
		{
			if(body != nullptr)
			{
				App->physics->SetBodyFlags(*body, gameObject->IsStatic(), kinematic);
			}
		}

		int tempType = (int)bodyType;
		if(ImGui::Combo("Body Type", &tempType, "Box\0Sphere\0Capsule"))
		{
			bodyType = (BodyType)tempType;
			if(body != nullptr)
			{
				App->physics->RebuildBody(*this);
			}
		}

		switch(bodyType)
		{
			case BodyType::SPHERE:
			{
				float& radius = ((SphereShapeInfo*)shapeInfo)->radius;
				if(ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 1000000.0f))
				{
					if(body != nullptr)
					{
						((btSphereShape*)body->getCollisionShape())->setUnscaledRadius(radius);
					}
				}
			}
			break;
			case BodyType::BOX:
			{
				float3& boxShape = ((BoxShapeInfo*)shapeInfo)->dimension;
				if(ImGui::DragFloat3("Dimension", boxShape.ptr(), 0.1f))
				{
					if(body != nullptr)
					{
						((btBoxShape*)body->getCollisionShape())->setImplicitShapeDimensions(btVector3(boxShape.x, boxShape.y, boxShape.z));
					}
				}
			}
			break;
			case BodyType::CAPSULE:
			{
				float& radius = ((CapsuleShapeInfo*)shapeInfo)->radius;
				float& height = ((CapsuleShapeInfo*)shapeInfo)->height;
				if(ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f, 1000000.0f))
				{
					if(body != nullptr)
					{
						((btCapsuleShape*)body->getCollisionShape())->setImplicitShapeDimensions(btVector3(radius, 0.5f * height, radius));
					}
				}
				if(ImGui::DragFloat("Height", &height, 0.1f, 0.0f, 1000000.0f))
				{
					if(body != nullptr)
					{
						((btCapsuleShape*)body->getCollisionShape())->setImplicitShapeDimensions(btVector3(radius, 0.5f * height, radius));
					}
				}
			}
			break;
		}

		ImGui::Text("Position Constraints");
		bool x = !linearFactor.x;
		if(ImGui::Checkbox("x", &x))
		{
			linearFactor.x = !x;
			if(body != nullptr)
			{
				App->physics->SetBodyConstraints(*body, linearFactor, angularFactor);
			}
		}
		ImGui::SameLine();
		bool y = !linearFactor.y;
		if(ImGui::Checkbox("y", &y))
		{
			linearFactor.y = !y;
			if(body != nullptr)
			{
				App->physics->SetBodyConstraints(*body, linearFactor, angularFactor);
			}
		}
		ImGui::SameLine();
		bool z = !linearFactor.z;
		if(ImGui::Checkbox("z", &z))
		{
			linearFactor.z = !z;
			if(body != nullptr)
			{
				App->physics->SetBodyConstraints(*body, linearFactor, angularFactor);
			}
		}

		ImGui::Text("Rotation Constraints");
		bool rx = !angularFactor.x;
		if(ImGui::Checkbox("x##Rot", &rx))
		{
			angularFactor.x = !rx;
			if(body != nullptr)
			{
				App->physics->SetBodyConstraints(*body, linearFactor, angularFactor);
			}
		}
		ImGui::SameLine();
		bool ry = !angularFactor.y;
		if(ImGui::Checkbox("y##Rot", &ry))
		{
			angularFactor.y = !ry;
			if(body != nullptr)
			{
				App->physics->SetBodyConstraints(*body, linearFactor, angularFactor);
			}
		}
		ImGui::SameLine();
		bool rz = !angularFactor.z;
		if(ImGui::Checkbox("z##Rot", &rz))
		{
			angularFactor.z = !rz;
			if(body != nullptr)
			{
				App->physics->SetBodyConstraints(*body, linearFactor, angularFactor);
			}
		}
	}
}

void RigidBody::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);
	bodyType = json["bodyType"];

	SetBodyType(bodyType);

	mass = json["mass"];

	switch(bodyType)
	{
		case BodyType::SPHERE:
			((SphereShapeInfo*)shapeInfo)->radius = json["shapeInfo"];
			break;
		case BodyType::BOX:
			from_json(json["shapeInfo"], ((BoxShapeInfo*)shapeInfo)->dimension);
			break;
		case BodyType::CAPSULE:
			((CapsuleShapeInfo*)shapeInfo)->radius = json["shapeInfo"]["radius"];
			((CapsuleShapeInfo*)shapeInfo)->height = json["shapeInfo"]["height"];
			break;
	}
}

void RigidBody::Save(nlohmann::json & json) const
{
	Component::Save(json);
	json["bodyType"] = bodyType;
	json["mass"] = mass;

	nlohmann::json jsonShapeInfo;

	switch(bodyType)
	{
		case BodyType::SPHERE:
			jsonShapeInfo = ((SphereShapeInfo*)shapeInfo)->radius;
			break;
		case BodyType::BOX:
			to_json(jsonShapeInfo, ((BoxShapeInfo*)shapeInfo)->dimension);
			break;
		case BodyType::CAPSULE:
			jsonShapeInfo["radius"] = ((CapsuleShapeInfo*)shapeInfo)->radius;
			jsonShapeInfo["height"] = ((CapsuleShapeInfo*)shapeInfo)->height;
			break;
	}

	json["shapeInfo"] = jsonShapeInfo;
}

void RigidBody::SetBodyType(BodyType newType)
{
	if(shapeInfo != nullptr)
	{
		RELEASE(shapeInfo);
	}

	bodyType = newType;

	switch(bodyType)
	{
		case BodyType::SPHERE:
			shapeInfo = new SphereShapeInfo();
			break;
		case BodyType::BOX:
			shapeInfo = new BoxShapeInfo();
			break;
		case BodyType::CAPSULE:
			shapeInfo = new CapsuleShapeInfo();
			break;
	}
}

void RigidBody::SetBody(btRigidBody* newBody)
{
	body = newBody;
}

BodyType RigidBody::GetBodyType() const
{
	return bodyType;
}

btRigidBody* RigidBody::GetBody() const
{
	return body;
}

float RigidBody::GetMass() const
{
	return mass;
}

bool RigidBody::GetKinematic() const
{
	return kinematic;
}

const float3 & RigidBody::GetLinearFactor() const
{
	return linearFactor;
}

const float3 & RigidBody::GetAngularFactor() const
{
	return angularFactor;
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

btCollisionShape* RigidBody::CreateCollisionShape() const
{
	btCollisionShape* colShape = nullptr;

	switch(bodyType)
	{
		case BodyType::SPHERE:
		{
			colShape = new btSphereShape(((SphereShapeInfo*)shapeInfo)->radius);
		}
			break;
		case BodyType::BOX:
		{
			float3 dimension = ((BoxShapeInfo*)shapeInfo)->dimension;
			colShape = new btBoxShape(btVector3(dimension.x, dimension.y, dimension.z));
		}
			break;
		case BodyType::CAPSULE:
		{
			colShape = new btCapsuleShape(((CapsuleShapeInfo*)shapeInfo)->radius, ((CapsuleShapeInfo*)shapeInfo)->height);
		}
			break;
	}

	return colShape;
}