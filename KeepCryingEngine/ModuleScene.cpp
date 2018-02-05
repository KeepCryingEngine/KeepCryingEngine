#include "ModuleScene.h"

#include <queue>

#include "Mesh.h"
#include "GameObject.h"

using namespace std;

ModuleScene::ModuleScene()
{ }

ModuleScene::~ModuleScene()
{ }

bool ModuleScene::Init()
{
	root = new GameObject("Root");

	return true;
}

update_status ModuleScene::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	CheckToDestroy();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float deltaTimeS, float realDeltaTimeS)
{
	CheckToStart();

	Update(root, deltaTimeS, realDeltaTimeS);

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	DestroyAndRelease(root);

	return true;
}

unsigned long long int ModuleScene::GetNewGameObjectId()
{
	return currentGameObjectId++;
}

GameObject* ModuleScene::GetRoot() const
{
	return root;
}

GameObject* ModuleScene::Get(unsigned long long int gameObjectId) const
{
	GameObject* toReturn = root->GetSelfOrChild(gameObjectId);
	if(toReturn == nullptr)
	{
		return root;
	}

	return root->GetSelfOrChild(gameObjectId);
}

GameObject* ModuleScene::AddEmpty(GameObject& parent, const char* name)
{
	GameObject* gameObject = new GameObject(name);
	gameObject->SetParent(parent);

	toStart.push_back(gameObject);

	return gameObject;
}

GameObject* ModuleScene::AddCube(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Cube");

	Mesh* mesh = (Mesh*)gameObject->AddComponent(ComponentType::Mesh);
	mesh->SetMeshMode(MeshMode::CUBE);

	return gameObject;
}

GameObject* ModuleScene::AddSphere(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Sphere");

	Mesh* mesh = (Mesh*)gameObject->AddComponent(ComponentType::Mesh);
	mesh->SetMeshMode(MeshMode::SPHERE);

	return gameObject;
}

void ModuleScene::Add(GameObject& gameObject)
{
	toStart.push_back(&gameObject);
}

void ModuleScene::Destroy(GameObject& gameObject)
{
	toDestroy.push_back(&gameObject);
}

void ModuleScene::Update(GameObject* gameObject, float deltaTimeS, float realDeltaTimeS) const
{
	// Iterative

	queue<GameObject*> toUpdate;
	toUpdate.push(gameObject);

	while(!toUpdate.empty())
	{
		GameObject* currentGameObject = toUpdate.front();
		toUpdate.pop();

		if(currentGameObject->IsEnabled())
		{
			currentGameObject->Update(deltaTimeS, realDeltaTimeS);

			for(GameObject* gameObjectChild : currentGameObject->GetChildren())
			{
				toUpdate.push(gameObjectChild);
			}
		}
	}

	/*

	// Recursive

	if(gameObject->IsEnabled())
	{
		gameObject->Update(deltaTimeS, realDeltaTimeS);

		for(GameObject* gameObjectChild : gameObject->GetChildren())
		{
			Update(gameObjectChild, deltaTimeS, realDeltaTimeS);
		}
	}

	*/
}

void ModuleScene::CheckToStart()
{
	for(GameObject* gameObject : toStart)
	{
		// gameObject->Start();
	}

	toStart.clear();
}

void ModuleScene::CheckToDestroy()
{
	for(GameObject* gameObject : toDestroy)
	{
		// If in toDestroy there are both P and C, where C is the child of P, then P will also release C
		// Then, it is necessary to check if the current game object has been released or not

		if(gameObject != nullptr)
		{
			// NO gameObject->GetParent()->RemoveChild(gameObject);
			
			// gameObject->SetParent(root);

			DestroyAndRelease(gameObject);
		}
	}

	toDestroy.clear();
}

void ModuleScene::DestroyAndRelease(GameObject* gameObject) const
{
	// Iterative

	// ...

	// Recursive

	for(GameObject* gameObjectChild : gameObject->GetChildren())
	{
		DestroyAndRelease(gameObjectChild);
	}

	if(gameObject->GetParent() != nullptr)
	{
		gameObject->GetParent()->DeleteChild(*gameObject);
	}

	gameObject->OnDestroy();

	RELEASE(gameObject);
}