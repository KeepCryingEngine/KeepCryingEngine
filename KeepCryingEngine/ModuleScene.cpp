#include "ModuleScene.h"

#include <queue>
#include <SDL_scancode.h>

#include "Mesh.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Transform.h"

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
	//CheckToStart();

	Update(root, deltaTimeS, realDeltaTimeS);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KeyState::KEY_DOWN)
	{
		qTGameObjects.Clear();

		qTGameObjects.Create(AABB2D(float2(-QUADTREE_SIZE, -QUADTREE_SIZE), float2(QUADTREE_SIZE, QUADTREE_SIZE)));

		AddToQuadtree(root);

		qTGameObjects.Print();
	}

	qTGameObjects.Draw();

	if(CHAOSAdded)
	{
		float valueXZ = QUADTREE_SIZE;
		float valueY = 5.0f;

		SetRandomPositionRecursive(root, -valueXZ, valueXZ, -valueY, valueY, -valueXZ, valueXZ);

		CHAOSAdded = false;
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	DestroyAndRelease(root);

	qTGameObjects.Clear();

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

	return toReturn;
}

GameObject* ModuleScene::AddEmpty(GameObject& parent, const char* name)
{
	GameObject* gameObject = new GameObject(name);
	gameObject->SetParent(parent);

	//toStart.push_back(gameObject);

	return gameObject;
}

GameObject* ModuleScene::AddCube(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Cube");

	Mesh* mesh = (Mesh*)gameObject->AddComponent(ComponentType::Mesh, true);
	mesh->SetMeshMode(MeshMode::CUBE);

	return gameObject;
}

GameObject* ModuleScene::AddSphere(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Sphere");

	Mesh* mesh = (Mesh*)gameObject->AddComponent(ComponentType::Mesh, true);
	mesh->SetMeshMode(MeshMode::SPHERE);

	return gameObject;
}

GameObject* ModuleScene::AddCamera(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Camera");

	gameObject->AddComponent(ComponentType::Camera, true);

	return gameObject;
}

void ModuleScene::AddCHAOS()
{
	size_t amount = 300;

	for(size_t i = 0; i < amount; ++i)
	{
		int gameObjectType = rand() % 2;

		GameObject* gameObject = nullptr;

		switch(gameObjectType)
		{
			case 0:
				gameObject = AddCube(*root);
				break;
			case 1:
				gameObject = AddSphere(*root);
				break;
		}
	}

	CHAOSAdded = true;
}

//void ModuleScene::Add(GameObject& gameObject)
//{
//	toStart.push_back(&gameObject);
//}

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

//void ModuleScene::CheckToStart()
//{
//	for(GameObject* gameObject : toStart)
//	{
//		// gameObject->Start();
//	}
//
//	toStart.clear();
//}

void ModuleScene::CheckToDestroy()
{
	for(GameObject* &gameObject : toDestroy)
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

void ModuleScene::DestroyAndRelease(GameObject* &gameObject) const
{
	// Iterative

	// ...

	// Recursive

	vector<GameObject*> childrenToDelete = gameObject->GetChildren();

	if(gameObject->GetParent() != nullptr)
	{
		gameObject->GetParent()->DeleteChildFromList(*gameObject);
	}

	for(GameObject* gameObjectChild : childrenToDelete)
	{
		DestroyAndRelease(gameObjectChild);
	}

	gameObject->OnDestroy();

	RELEASE(gameObject);
}

void ModuleScene::AddToQuadtree(GameObject* gameObject)
{
	qTGameObjects.Insert(gameObject);

	for(GameObject* gameObjectChild : gameObject->GetChildren())
	{
		AddToQuadtree(gameObjectChild);
	}
}

void ModuleScene::SetRandomPositionRecursive(GameObject* gameObject, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	if(gameObject != nullptr)
	{
		if(gameObject != root)
		{
			float x = minX + (float)(rand()) / ((float)(RAND_MAX / (maxX - minX)));
			float y = minY + (float)(rand()) / ((float)(RAND_MAX / (maxY - minY)));
			float z = minZ + (float)(rand()) / ((float)(RAND_MAX / (maxZ - minZ)));

			Transform* transform = ((Transform*)gameObject->GetComponent(ComponentType::Transform));
			transform->SetWorldPosition(float3(x, y, z));
		}

		for(GameObject* gameObjectChild : gameObject->GetChildren())
		{
			SetRandomPositionRecursive(gameObjectChild, minX, maxX, minY, maxY, minZ, maxZ);
		}
	}
}