#include "ModuleScene.h"

#include <queue>
#include <stack>
#include <SDL_scancode.h>
#include <LineSegment.h>

#include "MeshFilter.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Transform.h"
#include "Camera.h"
#include "ModuleCamera.h"
#include "ModuleUI.h"

using namespace std;

ModuleScene::ModuleScene()
{ }

ModuleScene::~ModuleScene()
{ }

bool ModuleScene::Init()
{
	root = new GameObject("Root");

	//qTGameObjects.Create(AABB(float3(-QUADTREE_SIZE, -QUADTREE_HEIGHT, -QUADTREE_SIZE), float3(QUADTREE_SIZE, QUADTREE_HEIGHT, QUADTREE_SIZE)));
	//oTGameObjects.Create(AABB(float3(-OCTREE_SIZE, -OCTREE_SIZE, -OCTREE_SIZE), float3(OCTREE_SIZE, OCTREE_SIZE, OCTREE_SIZE)));

	return true;
}

update_status ModuleScene::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	CheckToDestroy();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float deltaTimeS, float realDeltaTimeS)
{
	if(!App->ui->GetFrustumCulling())
	{
		// All visible

		SetVisibleRecursive(root, true);
	}
	else
	{
		// All invisible
		// If camera
		//    If spaceStructure
		//       Get game objects inside camera's frustum
		//       All previous game objects visible
		//    Else
		//       Check all game objects visibility

		SetVisibleRecursive(root, false);

		Camera* camera = App->camera->GetEnabledCamera();

		if(camera != nullptr)
		{
			if(spaceStructure == 0)
			{
				SetVisibilityRecursive(root);
			}
			else
			{
				vector<GameObject*> visibleGameObjects;

				if(spaceStructure == 1)
				{
					qTGameObjects.Intersect(visibleGameObjects, camera->GetFrustum());
				}
				else if(spaceStructure == 2)
				{
					oTGameObjects.Intersect(visibleGameObjects, camera->GetFrustum());
				}
				else if(spaceStructure == 3)
				{
					//kTGameObjects.Intersect(visibleGameObjects, camera->GetFrustum());
				}

				for(GameObject* visibleGameObject : visibleGameObjects)
				{
					visibleGameObject->SetVisible(true);
				}
			}
		}
	}

	// Generated game objects are already added to the scene
	// Set their position
	// Update aabb (mesh filter)
	// Set their static (if static, it is added to the qt)

	for(pair<GameObject*, pair<float3, bool>> generatedGameObject : generatedGameObjects)
	{
		GameObject* gameObject = generatedGameObject.first;
		const float3& worldPosition = generatedGameObject.second.first;
		gameObject->GetTransform()->SetWorldPosition(worldPosition);
	}

	Update(root, deltaTimeS, realDeltaTimeS);

	for(pair<GameObject*, pair<float3, bool>> generateGameObject : generatedGameObjects)
	{
		GameObject* gameObject = generateGameObject.first;
		bool isStatic = generateGameObject.second.second;
		gameObject->SetStatic(isStatic);
	}

	generatedGameObjects.clear();

	if(App->ui->GetDebugMode())
	{
		if(App->input->GetKey(SDL_SCANCODE_SPACE) == KeyState::KEY_DOWN)
		{
			if(spaceStructure == 1)
			{
				qTGameObjects.Print();
			}
			else if(spaceStructure == 2)
			{
				oTGameObjects.Print();
			}
			else if(spaceStructure == 3)
			{
				//kTGameObjects.Print();
			}
		}

		if(spaceStructure == 1)
		{
			qTGameObjects.Draw();
		}
		else if(spaceStructure == 2)
		{
			oTGameObjects.Draw();
		}
		else if(spaceStructure == 3)
		{
			//kTGameObjects.Draw();
		}
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	DestroyAndRelease(root);

	qTGameObjects.Clear();
	oTGameObjects.Clear();
	//kTGameObjects.Clear();

	return true;
}

int ModuleScene::GetNewGameObjectId()
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

void ModuleScene::Generate(int count, float staticPercentage, float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
	int staticCount = count * (int)Clamp01(0.01f * staticPercentage);

	for(size_t i = 0; i < (unsigned int)count; ++i)
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

		if(gameObject)
		{
			float x = minX + (float)(rand()) / ((float)(RAND_MAX / (maxX - minX)));
			float y = minY + (float)(rand()) / ((float)(RAND_MAX / (maxY - minY)));
			float z = minZ + (float)(rand()) / ((float)(RAND_MAX / (maxZ - minZ)));

			bool gameObjectStatic = false;

			if(staticCount > 0)
			{
				--staticCount;
				gameObjectStatic = true;
			}

			/* Transform* transform = gameObject->GetTransform();
			transform->SetWorldPosition(float3(x, y, z));
			gameObject->SetStatic(gameObjectStatic); */

			generatedGameObjects.push_back(pair<GameObject*, pair<float3, bool>>(gameObject, pair<float3, bool>(float3(x, y, z), gameObjectStatic)));
		}
	}
}

void ModuleScene::Destroy(GameObject& gameObject)
{
	toDestroy.push_back(&gameObject);
}

void ModuleScene::AddStatic(GameObject* gameObject)
{
	qTGameObjects.Insert(gameObject);
	oTGameObjects.Insert(gameObject);
	//kTGameObjects.Insert(gameObject);
}

void ModuleScene::RemoveStatic(GameObject* gameObject)
{
	qTGameObjects.Remove(gameObject);
	oTGameObjects.Remove(gameObject);
	//kTGameObjects.Remove(gameObject);
}

void ModuleScene::SetSpacePartitioningStructure(int spacePartitioningStructure)
{
	spaceStructure = spacePartitioningStructure;
}

bool ModuleScene::RayCast(const float3& origin, const float3& direction, float maxDistance, RayCastHit& rayCastHit) const
{
	InitializeRayCastHit(rayCastHit);
	LineSegment lineSegment = BuildLineSegmentForRayCast(origin, direction, maxDistance);

	bool hit = false;
	stack<GameObject*> gameObjects;
	while(!gameObjects.empty())
	{
		GameObject* currentGameObject = gameObjects.top();
		gameObjects.pop();

		for (GameObject * child : currentGameObject->GetChildren())
		{
			gameObjects.push(child);
		}

		hit = RayCastGameObject(currentGameObject, lineSegment, rayCastHit) || hit;
	}

	return hit;
}

LineSegment ModuleScene::BuildLineSegmentForRayCast(const math::float3 & origin, const math::float3 & direction, float maxDistance) const
{
	LineSegment lineSegment;
	lineSegment.a = origin;
	lineSegment.b = direction.Normalized() * maxDistance;
	return lineSegment;
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

void ModuleScene::InitializeRayCastHit(RayCastHit & rayCastHit) const 
{
	rayCastHit.gameObject = nullptr;
	rayCastHit.normal = float3::zero;
	rayCastHit.point = float3::zero;
	rayCastHit.distance = INFINITY;
	rayCastHit.normalizedDistance = INFINITY;
}

bool ModuleScene::RayCastGameObject(GameObject * gameObject, const LineSegment & lineSegment, RayCastHit& rayCastHit) const
{
	bool hit = false;
	MeshFilter* meshFilter = (MeshFilter*)gameObject->GetComponent(ComponentType::MeshFilter);
	if (meshFilter)
	{
		Mesh* mesh = meshFilter->GetMesh();
		if (mesh)
		{
			hit = RayCastMesh(gameObject, mesh, lineSegment, rayCastHit);
		}
	}
	return hit;
}

bool ModuleScene::RayCastMesh(GameObject* gameObject, Mesh * mesh, const LineSegment & lineSegment, RayCastHit& rayCastHit) const
{
	bool hit = false;
	if (mesh->GetDrawMode() == GL_TRIANGLES)
	{
		Triangle triangle;
		float3 point;
		float normalizedDistance;
		size_t currentIndex = 0;
		for (size_t i = 0; i < 3; i++)
		{
			triangle.a = mesh->GetVertices().at(currentIndex++).position;
			triangle.b = mesh->GetVertices().at(currentIndex++).position;
			triangle.c = mesh->GetVertices().at(currentIndex++).position;
			if (lineSegment.Intersects(triangle, &normalizedDistance, &point))
			{
				if (normalizedDistance < rayCastHit.normalizedDistance)
				{
					rayCastHit.gameObject = gameObject;
					rayCastHit.normalizedDistance = normalizedDistance;
					rayCastHit.point = point;
					rayCastHit.distance = lineSegment.a.Distance(point);
					rayCastHit.normal = triangle.NormalCCW();
					hit = true;
				}
			}
		}
	}
	return hit;
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