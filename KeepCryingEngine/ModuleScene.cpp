#include "ModuleScene.h"

#include <queue>

#include "GameObject.h"

using namespace std;

ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	root = new GameObject();
	return true;
}

bool ModuleScene::CleanUp()
{
	RELEASE(root);
	return true;
}

update_status ModuleScene::Update(float deltaTimeS, float realDeltaTimeS)
{
	for each (GameObject* o in startList)
	{
		o->Start();
	}
	startList.clear();

	queue<GameObject*> updateList;
	updateList.push(root);

	while(!updateList.empty())
	{
		GameObject* temp = updateList.front();
		updateList.pop();
		temp->Update(deltaTimeS,realDeltaTimeS);
		vector<GameObject*> tempChilds = temp->GetChilds();
		for each (GameObject* o in tempChilds)
		{
			updateList.push(o);
		}
	}

	return update_status::UPDATE_CONTINUE;
}

GameObject& ModuleScene::GetRoot() const
{
	return *root;
}

void ModuleScene::AddToStartList(GameObject& object)
{
	startList.push_back(&object);
}