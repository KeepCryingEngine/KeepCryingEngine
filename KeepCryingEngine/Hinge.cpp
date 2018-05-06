#include "Hinge.h"

#include "GameObject.h"
#include "RigidBody.h"
#include "Application.h"
#include "ModulePhysics.h"

Hinge::Hinge():Component(TYPE)
{}

Hinge::~Hinge()
{}

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
				for(int i = 0; i < children.size(); ++i)
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
{}

void Hinge::ApplyConstraint(btRigidBody & ownBody)
{}

void Hinge::ApplyConstraint(btRigidBody & ownBody, btRigidBody & secondBody)
{}
