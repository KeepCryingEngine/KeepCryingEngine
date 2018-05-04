#include "RigidBody.h"

#include "GameObject.h"
#include "Transform.h"

RigidBody::RigidBody():Component(TYPE)
{}

RigidBody::~RigidBody()
{}

void RigidBody::Awake()
{}

void RigidBody::Destroy()
{}

void RigidBody::DrawUI()
{}

void RigidBody::PreLoad(const nlohmann::json & json)
{}

void RigidBody::Save(nlohmann::json & json) const
{}

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

	Quat tempQuat(rot.getX(),rot.getY(),rot.getZ(),rot.getW());
	float3 tempVec(pos.getX(),pos.getY(),pos.getZ());
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
