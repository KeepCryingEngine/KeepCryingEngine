#include "Slider.h"

#include "GameObject.h"
#include "RigidBody.h"
#include "Application.h"
#include "ModulePhysics.h"

Slider::Slider():Component(TYPE)
{}

Slider::~Slider()
{}

void Slider::RealUpdate()
{
	if(App->state != TimeState::STOPED)
	{
		btRigidBody* ownBody = gameObject->GetComponent<RigidBody>()->GetBody();
		if(constraint == nullptr &&  ownBody != nullptr)
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
	}
}

void Slider::Destroy()
{
	if(constraint != nullptr)
	{
		App->physics->GetWorld()->removeConstraint(constraint);
	}
}

void Slider::DrawUI()
{
	if(ImGui::CollapsingHeader("Slider"))
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
			if(constraint != nullptr)
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
	}
}

void Slider::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);

	from_json(json["ownPivot"], ownPivot);
	from_json(json["ownAngle"], ownAngle);

	from_json(json["secondPivot"], secondPivot);
	from_json(json["secondAngle"], secondAngle);
}

void Slider::Save(nlohmann::json & json) const
{
	Component::Save(json);

	to_json(json["ownPivot"], ownPivot);
	to_json(json["ownAngle"], ownAngle);

	to_json(json["secondPivot"], secondPivot);
	to_json(json["secondAngle"], secondAngle);
}

void Slider::ApplyConstraint(btRigidBody & ownBody, btRigidBody & secondBody)
{
	if(ownAngle.x <= 0.0f && ownAngle.y <= 0.0f && ownAngle.z <= 0.0f)
	{
		ownAngle = float3::one;
	}
	if(secondAngle.x <= 0.0f && secondAngle.y <= 0.0f && secondAngle.z <= 0.0f)
	{
		secondAngle = float3::one;
	}

	btTransform frameA;
	frameA.setOrigin(btVector3(ownPivot.x, ownPivot.y, ownPivot.z));
	frameA.getBasis().setEulerZYX(ownAngle.x, ownAngle.y, ownAngle.z);

	btTransform frameB;
	frameB.setOrigin(btVector3(secondPivot.x, secondPivot.y, secondPivot.z));
	frameB.getBasis().setEulerZYX(secondAngle.x, secondAngle.y, secondAngle.z);

	constraint = new btSliderConstraint(ownBody,secondBody,frameA,frameB,false);
	App->physics->GetWorld()->addConstraint(constraint);
}

std::vector<Component::Type> Slider::GetProhibitedComponents() const
{
	return { Slider::TYPE };
}

std::vector<Component::Type> Slider::GetNeededComponents() const
{
	return { RigidBody::TYPE };
}