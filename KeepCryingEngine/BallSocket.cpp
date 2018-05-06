#include "BallSocket.h"

#include "GameObject.h"
#include "RigidBody.h"
#include "Application.h"
#include "ModulePhysics.h"

BallSocket::BallSocket():Component(TYPE)
{}

BallSocket::~BallSocket()
{}

void BallSocket::RealUpdate()
{
	if(App->state != TimeState::STOPED)
	{
		btRigidBody* ownBody = gameObject->GetComponent<RigidBody>()->GetBody();
		if(constraint == nullptr &&  ownBody != nullptr)
		{
			if(usingSecond)
			{
				const std::vector<GameObject*>& children = gameObject->GetChildren();
				for(int i = 0; i < children.size(); ++i)
				{
					if(children[i]->GetComponent<RigidBody>() != nullptr)
					{
						btRigidBody* secondBody = children[i]->GetComponent<RigidBody>()->GetBody();
						if(secondBody != nullptr)
						{
							ApplyConstraint(*ownBody,*secondBody);
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

void BallSocket::DrawUI()
{
	if(ImGui::CollapsingHeader("BallSocket"))
	{
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
		if(ImGui::DragFloat3("My Pivot", ownPivot.ptr(), 0.1f))
		{
			if(constraint != nullptr)
			{
				constraint->setPivotA(btVector3(ownPivot.x, ownPivot.y, ownPivot.z));
			}
		}
		if(ImGui::DragFloat3("Second Pivot", secondPivot.ptr(), 0.1f))
		{
			if(constraint != nullptr && usingSecond)
			{
				constraint->setPivotB(btVector3(secondPivot.x, secondPivot.y, secondPivot.z));
			}
		}
		if(App->state == TimeState::STOPED)
		{
			ImGui::Checkbox("Use second Body", &usingSecond);
		}
		
	}

}

void BallSocket::ApplyConstraint(btRigidBody& ownBody)
{	
	constraint = new btPoint2PointConstraint(ownBody, btVector3(ownPivot.x, ownPivot.y, ownPivot.z));
	App->physics->GetWorld()->addConstraint(constraint);
}

void BallSocket::ApplyConstraint(btRigidBody & ownBody, btRigidBody & secondBody)
{
	constraint = new btPoint2PointConstraint(ownBody, secondBody, btVector3(ownPivot.x, ownPivot.y, ownPivot.z), btVector3(secondPivot.x, secondPivot.y, secondPivot.z));
	App->physics->GetWorld()->addConstraint(constraint);
}
