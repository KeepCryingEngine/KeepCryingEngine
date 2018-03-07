#include "ModuleGameUI.h"

#include <vector>

#include "GameObject.h"
#include "Canvas.h"

using namespace std;

ModuleGameUI::ModuleGameUI()
{}

ModuleGameUI::~ModuleGameUI()
{}

update_status ModuleGameUI::Update(float deltaTimeS, float realDeltaTimeS)
{
	if(root != nullptr)
	{
		UpdateRecursive(deltaTimeS, realDeltaTimeS, root->gameObject);
	}
	return update_status::UPDATE_CONTINUE;
}

bool ModuleGameUI::CleanUp()
{
	return true;
}

void ModuleGameUI::UpdateRecursive(float deltaTimeS, float realDeltaTimeS, GameObject * g)
{
	vector<Component*> components = g->GetComponents();
	for(vector<Component*>::iterator it = components.begin(); it != components.end(); it++)
	{
		if(!(*it)->enabled)
		{
			continue;
		}

		switch((*it)->type)
		{
			case Component::Type::Canvas:
			{

			}
			break;
			case Component::Type::Text:
			{

			}
			break;
			case Component::Type::Button:
			{

			}
			break;
			case Component::Type::InpuText:
			{

			}
			break;
		}
	}

	vector<GameObject*> childs = g->GetChildren();
	for(vector<GameObject*>::iterator it = childs.begin();it != childs.end();it++)
	{
		UpdateRecursive(deltaTimeS,realDeltaTimeS,(*it));
	}
}
