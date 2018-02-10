#include "ModuleScene.h"

#include <queue>
#include <SDL_scancode.h>

#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Transform.h"
#include "Camera.h"
#include "ModuleCamera.h"

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
	SetVisibleRecursive(root, false);

	Camera* camera = App->camera->GetEnabledCamera();

	if(camera != nullptr)
	{
		if(useQuadtree)
		{
			vector<GameObject*> visibleGameObjects;
			qTGameObjects.Intersect(visibleGameObjects, camera->GetFrustum());

			for(GameObject* visibleGameObject : visibleGameObjects)
			{
				visibleGameObject->SetVisible(true);
			}
		}
		else
		{
			SetVisibilityRecursive(root);
		}
	}

	Update(root, deltaTimeS, realDeltaTimeS);

	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KeyState::KEY_DOWN)
	{
		qTGameObjects.Clear();

		qTGameObjects.Create(AABB(float3(-QUADTREE_SIZE, -1011, -QUADTREE_SIZE), float3(QUADTREE_SIZE, 1011, QUADTREE_SIZE)));

		AddToQuadtreeRecursive(root);

		qTGameObjects.Print();
	}

	qTGameObjects.Draw();

	for(pair<GameObject*, float3> generateGameObject : generatedGameObjects)
	{
		Transform* transform = ((Transform*)generateGameObject.first->GetComponent(ComponentType::Transform));
		transform->SetWorldPosition(generateGameObject.second);
	}

	generatedGameObjects.clear();

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

	return gameObject;
}

GameObject* ModuleScene::AddCube(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Cube");

	gameObject->AddComponent(ComponentType::MeshRenderer);

	return gameObject;
}

GameObject* ModuleScene::AddSphere(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Sphere");

	gameObject->AddComponent(ComponentType::MeshRenderer);
	MeshFilter* temp= ((MeshFilter*)gameObject->GetComponent(ComponentType::MeshFilter));
	assert(temp);
	temp->SetMeshMode(MeshMode::SPHERE);


	return gameObject;
}

GameObject* ModuleScene::AddCamera(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Camera");

	gameObject->AddComponent(ComponentType::Camera, true);

	return gameObject;
}

void ModuleScene::Generate(int count, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	for(size_t i = 0; i < count; ++i)
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

		float x = minX + (float)(rand()) / ((float)(RAND_MAX / (maxX - minX)));
		float y = minY + (float)(rand()) / ((float)(RAND_MAX / (maxY - minY)));
		float z = minZ + (float)(rand()) / ((float)(RAND_MAX / (maxZ - minZ)));

		generatedGameObjects.push_back(pair<GameObject*, float3>(gameObject, float3(x, y, z)));
	}
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

void ModuleScene::AddToQuadtreeRecursive(GameObject* gameObject)
{
	qTGameObjects.Insert(gameObject);

	for(GameObject* gameObjectChild : gameObject->GetChildren())
	{
		AddToQuadtreeRecursive(gameObjectChild);
	}
}

void ModuleScene::SetVisibleRecursive(GameObject* gameObject, bool visible) const
{
	gameObject->SetVisible(visible);

	for(GameObject* child : gameObject->GetChildren())
	{
		SetVisibleRecursive(child, visible);
	}
}

void ModuleScene::SetVisibilityRecursive(GameObject* gameObject) const
{
	gameObject->SetVisible(Camera::Intersects(App->camera->GetEnabledCamera()->GetFrustum(), gameObject->GetAABB()));

	for(GameObject* child : gameObject->GetChildren())
	{
		SetVisibilityRecursive(child);
	}
}