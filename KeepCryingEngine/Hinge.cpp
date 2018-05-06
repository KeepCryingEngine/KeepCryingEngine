#include "Hinge.h"

#include "GameObject.h"
#include "RigidBody.h"
#include "Application.h"
#include "ModulePhysics.h"

Hinge::Hinge():Component(TYPE)
{ }

Hinge::~Hinge()
{ }

void Hinge::RealUpdate()
{
	if(App->state != TimeState::STOPED)
	{
		btRigidBody* ownBody = gameObject->GetComponent<RigidBody>()->GetBody();
		if(constraint == nullptr &&  ownBody != nullptr)
		{
			if(usingSecond)
			{
				const std::vector<GameObject*>& children = gameObject->GetChildren();
				for(size_t i = 0; i < children.size(); ++i)
				{
					if(children[i]->GetComponent<RigidBody>() != nullptr)
					{
						btRigidBody* secondBody = children[i]->GetComponent<RigidBody>()->GetBody();
						if(secondBody != nullptr)
						{
							ApplyConstraint(*ownBody, *secondBody);
						}
						break;
					}
				}
			}
			else
			{
				ApplyConstraint(*ownBody);
			}
		}
	}
}

void Hinge::DrawUI()
{
	if(ImGui::CollapsingHeader("Hinge"))
	{
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
		if(ImGui::DragFloat3("My Pivot", ownPivot.ptr(), 0.1f))
		{
			if(constraint != nullptr)
			{
				constraint->getFrameOffsetA().setOrigin(btVector3(ownPivot.x, ownPivot.y, ownPivot.z));
			}
		}
		if(ImGui::DragFloat3("My Angle", ownAngle.ptr(), 0.1f))
		{
			if(!(ownAngle.x <= 0.0f && ownAngle.y <= 0.0f && ownAngle.z <= 0.0f))
			{
				if(constraint != nullptr)
				{
					btMatrix3x3& basis = constraint->getFrameOffsetA().getBasis();
					basis[0][2] = ownAngle.x;
					basis[1][2] = ownAngle.y;
					basis[2][2] = ownAngle.z;
				}
			}
		}
		if(ImGui::DragFloat3("Second Pivot", secondPivot.ptr(), 0.1f))
		{
			if(constraint != nullptr && usingSecond)
			{
				constraint->getFrameOffsetB().setOrigin(btVector3(secondPivot.x, secondPivot.y, secondPivot.z));
			}
		}
		if(ImGui::DragFloat3("Second Angle", secondAngle.ptr(), 0.1f))
		{
			if(!(secondAngle.x <= 0.0f && secondAngle.y <= 0.0f && secondAngle.z <= 0.0f))
			{
				if(constraint != nullptr)
				{
					btMatrix3x3& basis = constraint->getFrameOffsetB().getBasis();
					basis[0][2] = secondAngle.x;
					basis[1][2] = secondAngle.y;
					basis[2][2] = secondAngle.z;
				}
			}
		}
		if(App->state == TimeState::STOPED)
		{
			ImGui::Checkbox("Use second Body", &usingSecond);
		}
	}
}

void Hinge::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);

	from_json(json["ownPivot"], ownPivot);
	from_json(json["ownAngle"], ownAngle);

	from_json(json["secondPivot"], secondPivot);
	from_json(json["secondAngle"], secondAngle);

	usingSecond = json["usingSecond"];
}

void Hinge::Save(nlohmann::json & json) const
{
	Component::Save(json);

	to_json(json["ownPivot"], ownPivot);
	to_json(json["ownAngle"], ownAngle);

	to_json(json["secondPivot"], secondPivot);
	to_json(json["secondAngle"], secondAngle);

	json["usingSecond"] = usingSecond;
}

void Hinge::ApplyConstraint(btRigidBody & ownBody)
{
	if(ownAngle.x <= 0.0f && ownAngle.y <= 0.0f && ownAngle.z <= 0.0f)
	{
		ownAngle = float3::one;
	}
	constraint = new btHingeConstraint(ownBody,	btVector3(ownPivot.x, ownPivot.y, ownPivot.z),btVector3(ownAngle.x, ownAngle.y, ownAngle.z));
	App->physics->GetWorld()->addConstraint(constraint);
}

void Hinge::ApplyConstraint(btRigidBody & ownBody, btRigidBody & secondBody)
{
	if(ownAngle.x <= 0.0f && ownAngle.y <= 0.0f && ownAngle.z <= 0.0f)
	{
		ownAngle = float3::one;
	}
	if(secondAngle.x <= 0.0f && secondAngle.y <= 0.0f && secondAngle.z <= 0.0f)
	{
		secondAngle = float3::one;
	}
	constraint = new btHingeConstraint(ownBody, secondBody, btVector3(ownPivot.x, ownPivot.y, ownPivot.z),btVector3(secondPivot.x, secondPivot.y, secondPivot.z), btVector3(ownAngle.x, ownAngle.y, ownAngle.z),btVector3(secondAngle.x, secondAngle.y, secondAngle.z));
	App->physics->GetWorld()->addConstraint(constraint);
}