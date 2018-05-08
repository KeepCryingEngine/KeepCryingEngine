#include "ModuleScene.h"

#include <queue>
#include <stack>
#include <ImGuizmo.h>
#include <SDL_scancode.h>
#include <LineSegment.h>
#include <fstream>

#include "MeshFilter.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Transform.h"
#include "Camera.h"
#include "ModuleCamera.h"
#include "ModuleEditorUI.h"
#include "Animator.h"
#include "AudioSource.h"
#include "ModuleGameUI.h"
#include "Transform2D.h"
#include "Canvas.h"
#include "Image.h"
#include "Button.h"
#include "Text.h"
#include "InputText.h"

using namespace std;
using namespace nlohmann;

ModuleScene::ModuleScene()
{ }

ModuleScene::~ModuleScene()
{ }

bool ModuleScene::Init()
{
	root = new GameObject("Root");

	root->SetId(0);

	return true;
}

update_status ModuleScene::PreUpdate()
{
	CheckToDestroy();

	return update_status::UPDATE_CONTINUE;
}

#include "Application.h"
#include "Script.h"

update_status ModuleScene::Update()
{
	if(App->uiEditor->GetDebugMode())
	{
		DrawHierarchy(Get(App->uiEditor->GetSelectedNode()));
	}

	if(!App->uiEditor->GetFrustumCulling())
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
					kTGameObjects.Intersect(visibleGameObjects, camera->GetFrustum());
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

	Update(root);

	for(pair<GameObject*, pair<float3, bool>> generateGameObject : generatedGameObjects)
	{
		GameObject* gameObject = generateGameObject.first;
		bool isStatic = generateGameObject.second.second;
		gameObject->SetStatic(isStatic);
	}

	generatedGameObjects.clear();

	if(App->uiEditor->GetDebugMode())
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
				kTGameObjects.Print();
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
			kTGameObjects.Draw();
		}
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	DestroyAndRelease(root);

	qTGameObjects.Clear();
	oTGameObjects.Clear();
	kTGameObjects.Clear();
	dGameobjects.clear();

	return true;
}

/* int ModuleScene::GetNewGameObjectId()
{
	return currentGameObjectId++;
} */

GameObject* ModuleScene::GetRoot() const
{
	return root;
}

GameObject* ModuleScene::Get(int gameObjectId) const
{
	GameObject* gameObject = root->GetById(gameObjectId);
	if(gameObject == nullptr)
	{
		return root;
	}

	return gameObject;
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

	gameObject->AddComponent<MeshRenderer>();

	return gameObject;
}

GameObject* ModuleScene::AddSphere(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Sphere");

	gameObject->AddComponent<MeshRenderer>();
	MeshFilter* temp= gameObject->GetComponent<MeshFilter>();
	assert(temp);
	temp->SetMeshMode(MeshMode::SPHERE);

	return gameObject;
}

GameObject * ModuleScene::AddPlane(GameObject & parent)
{
	GameObject* gameObject = AddEmpty(parent, "Plane");

	gameObject->AddComponent<MeshRenderer>();
	MeshFilter* temp = gameObject->GetComponent<MeshFilter>();
	assert(temp);
	temp->SetMeshMode(MeshMode::PLANE);

	return gameObject;
}

GameObject* ModuleScene::AddCamera(GameObject& parent)
{
	GameObject* gameObject = AddEmpty(parent, "Camera");

	gameObject->AddComponent<Camera>();

	return gameObject;
}

GameObject * ModuleScene::AddCanvas(GameObject & parent)
{
	GameObject* gameObject = AddEmpty(parent, "Canvas");

	gameObject->AddComponent<Transform2D>();
	gameObject->AddComponent<Canvas>();
	return gameObject;
}

GameObject * ModuleScene::AddImage(GameObject & parent)
{
	GameObject* gameObject;
	switch(IsChildOfCanvas(parent))
	{
		case uiState::NO_CANVAS:
		{
			gameObject = AddEmpty(*AddCanvas(parent), "Image");
		}
		break;
		case uiState::NO_FATHER_CANVAS:
		{
			gameObject = AddEmpty(*App->uiGame->GetCanvas()->gameObject, "Image");
		}
		break;
		case uiState::FATHER_CANVAS:
		{
			gameObject = AddEmpty(parent, "Image");
		}
		break;
	}
	gameObject->AddComponent<Transform2D>();
	gameObject->AddComponent<Image>();
	return gameObject;
}

GameObject * ModuleScene::AddButton(GameObject & parent)
{
	GameObject* gameObject;
	switch(IsChildOfCanvas(parent))
	{
		case uiState::NO_CANVAS:
		{
			gameObject = AddEmpty(*AddCanvas(parent), "Button");
		}
			break;
		case uiState::NO_FATHER_CANVAS:
		{
			gameObject = AddEmpty(*App->uiGame->GetCanvas()->gameObject, "Button");
		}
			break;
		case uiState::FATHER_CANVAS:
		{
			gameObject = AddEmpty(parent, "Button");
		}
		break;
	}
	gameObject->AddComponent<Transform2D>();
	gameObject->AddComponent<Image>();
	gameObject->AddComponent<Button>();
	AddText(*gameObject);
	return gameObject;
}

GameObject * ModuleScene::AddText(GameObject & parent)
{
	GameObject* gameObject;
	switch(IsChildOfCanvas(parent))
	{
		case uiState::NO_CANVAS:
		{
			gameObject = AddEmpty(*AddCanvas(parent), "Text");
		}
		break;
		case uiState::NO_FATHER_CANVAS:
		{
			gameObject = AddEmpty(*App->uiGame->GetCanvas()->gameObject, "Text");
		}
		break;
		case uiState::FATHER_CANVAS:
		{
			gameObject = AddEmpty(parent, "Text");
		}
		break;
	}
	gameObject->AddComponent<Transform2D>();
	gameObject->AddComponent<Text>();
	return gameObject;
}

GameObject * ModuleScene::AddInputText(GameObject & parent)
{
	GameObject* gameObject;
	switch(IsChildOfCanvas(parent))
	{
		case uiState::NO_CANVAS:
		{
			gameObject = AddEmpty(*AddCanvas(parent), "InputText");
		}
		break;
		case uiState::NO_FATHER_CANVAS:
		{
			gameObject = AddEmpty(*App->uiGame->GetCanvas()->gameObject, "InputText");
		}
		break;
		case uiState::FATHER_CANVAS:
		{
			gameObject = AddEmpty(parent, "InputText");
		}
		break;
	}
	gameObject->AddComponent<Transform2D>();
	gameObject->AddComponent<Image>();
	InputText* tempInput = gameObject->AddComponent<InputText>();
	tempInput->SetPlaceHolderGameObject(*AddText(*gameObject));//Add text in a sub-object
	tempInput->SetTextGameObject(*AddText(*gameObject));//Add text in a sub-object
	tempInput->SetCursorGameObject(*AddImage(*gameObject));//Add Image in a sub-object

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

			generatedGameObjects.push_back(pair<GameObject*, pair<float3, bool>>(gameObject, pair<float3, bool>(float3(x, y, z), gameObjectStatic)));
		}
	}
}

void ModuleScene::Destroy(GameObject& gameObject)
{
	if(gameObject.IsStatic())
	{
		qTGameObjects.Remove(&gameObject);
		oTGameObjects.Remove(&gameObject);
		kTGameObjects.Remove(&gameObject);
	}
	else
	{
		dGameobjects.erase(find(dGameobjects.begin(), dGameobjects.end(), &gameObject));
	}
	toDestroy.push_back(&gameObject);
}

void ModuleScene::AddStatic(GameObject* gameObject)
{
	qTGameObjects.Insert(gameObject);
	oTGameObjects.Insert(gameObject);
	kTGameObjects.Insert(gameObject);
	dGameobjects.erase(find(dGameobjects.begin(),dGameobjects.end(),gameObject));
}

void ModuleScene::RemoveStatic(GameObject* gameObject)
{
	qTGameObjects.Remove(gameObject);
	oTGameObjects.Remove(gameObject);
	kTGameObjects.Remove(gameObject);
	dGameobjects.push_back(gameObject);
}

void ModuleScene::SetSpacePartitioningStructure(int spacePartitioningStructure)
{
	spaceStructure = spacePartitioningStructure;
}

LineSegment ModuleScene::BuildLineSegmentForRayCast(const math::float3 & origin, const math::float3 & direction, float maxDistance) const
{
	LineSegment lineSegment;
	lineSegment.a = origin;
	lineSegment.b = origin + direction.Normalized() * maxDistance;
	return lineSegment;
}

LineSegment ModuleScene::ProjectLineSegmentToGameObjectsLocalSpace(const LineSegment & worldSpaceLineSegment, const GameObject& gameObject) const
{
	LineSegment gameObjectSpaceLineSegment(worldSpaceLineSegment);
	gameObjectSpaceLineSegment.Transform(gameObject.GetTransform()->GetModelMatrix().Inverted());
	return gameObjectSpaceLineSegment;
}

void ModuleScene::Update(GameObject* gameObject) const
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
			currentGameObject->Update();

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

GameObject* ModuleScene::AddEmptyEmpty(GameObject& parent, const char* name)
{
	GameObject* gameObject = new GameObject(name, true);
	gameObject->SetParent(parent);

	return gameObject;
}

void ModuleScene::InitializeRayCastHit(RayCastHit & rayCastHit) const
{
	rayCastHit.gameObject = nullptr;
	rayCastHit.normal = float3::zero;
	rayCastHit.point = float3::zero;
	rayCastHit.distance = INFINITY;
	rayCastHit.normalizedDistance = INFINITY;
}

bool ModuleScene::RayCast(const float3& origin, const float3& direction, float maxDistance, RayCastHit& rayCastHit) const
{
	bool hit = false;

	vector<RayCastHit> hitVector= RayCastAll(origin,direction,maxDistance);
	if(!hitVector.empty())
	{
		rayCastHit = hitVector[0];
		hit = true;
	}
	return hit;
}

std::vector<RayCastHit> ModuleScene::RayCastAll(const float3 & origin, const float3 & direction, float maxDistance) const
{
	vector<RayCastHit> rayCastHits;

	RayCastHit rayCastHit;
	InitializeRayCastHit(rayCastHit);

	LineSegment worldSpaceLineSegment = BuildLineSegmentForRayCast(origin, direction, maxDistance);

	vector<GameObject*> gameObjects;
	kTGameObjects.Intersect(gameObjects, worldSpaceLineSegment);

	for(GameObject* g : dGameobjects)
	{
		if(worldSpaceLineSegment.Intersects(g->GetAABB()))
		{
			gameObjects.push_back(g);
		}
	}

	for(GameObject* gameObject : gameObjects)
	{
		bool hit = RayCastGameObject(gameObject, worldSpaceLineSegment, rayCastHit);

		if(hit)
		{
			rayCastHits.push_back(rayCastHit);
			InitializeRayCastHit(rayCastHit);
		}
	}

	/* stack<GameObject*> gameObjects;
	gameObjects.push(root);

	while(!gameObjects.empty())
	{
		GameObject* currentGameObject = gameObjects.top();
		gameObjects.pop();

		for (GameObject * child : currentGameObject->GetChildren())
		{
			gameObjects.push(child);
		}
		
		bool hit = RayCastGameObject(currentGameObject, worldSpaceLineSegment, rayCastHit);
		if (hit)
		{
			rayCastHits.push_back(rayCastHit);
			InitializeRayCastHit(rayCastHit);
		}
	} */

	auto sortRayCastFunction = [](const RayCastHit& a, const RayCastHit& b) -> bool { return a.normalizedDistance < b.normalizedDistance; };
	sort(rayCastHits.begin(), rayCastHits.end(), sortRayCastFunction);

	return rayCastHits;
}

void ModuleScene::AddToDinamicGameobjectList(GameObject* gameobject)
{
	dGameobjects.push_back(gameobject);
}

void ModuleScene::Restore()
{
	if(validJsonData)
	{
		LoadScene(jsonData);
	}
}

void ModuleScene::LoadFromFile(const char* fileName)
{
	json jsonScene;

	string path = "Assets/Scenes/";
	path += fileName;
	path += ".scene";

	ifstream file(path);

	if(!file.is_open())
	{
		return;
	}

	file >> jsonScene;
	file.close();

	LoadScene(jsonScene);
}

void ModuleScene::Save()
{
	SaveScene(jsonData);

	validJsonData = true;
}

void ModuleScene::SaveToFile(const char* fileName) const
{
	json jsonScene;

	SaveScene(jsonScene);

	string path = "Assets/Scenes/";
	path += fileName;
	path += ".scene";

	ofstream file(path);
	file << jsonScene;
	file.close();
}

bool ModuleScene::RayCastGameObject(GameObject * gameObject, const LineSegment & worldSpaceLineSegment, RayCastHit& rayCastHit) const
{
	bool hit = false;

	if (worldSpaceLineSegment.Intersects(gameObject->GetAABB()))
	{
		MeshFilter* meshFilter = gameObject->GetComponent<MeshFilter>();
		if (meshFilter)
		{
			Mesh* mesh = meshFilter->GetMesh();
			if (mesh)
			{
				hit = RayCastMesh(gameObject, mesh, worldSpaceLineSegment, rayCastHit);
			}
		}
	}
	
	return hit;
}

bool NextMeshTriangle(Mesh* mesh, Triangle& triangle, size_t& index);

void BuildRayCastHit(RayCastHit& rayCastHit, GameObject* gameObject, float3 point, float normalizedDistance, const LineSegment& localGameObjectSpaceLineSegment, const Triangle& triangle)
{
	const float4x4& gameObjectModelMatrix = gameObject->GetTransform()->GetModelMatrix();
	rayCastHit.gameObject = gameObject;
	rayCastHit.normalizedDistance = normalizedDistance;
	rayCastHit.point = gameObjectModelMatrix.TransformPos(point);
	rayCastHit.distance = localGameObjectSpaceLineSegment.a.Distance(point);
	rayCastHit.normal = gameObjectModelMatrix.TransformDir(triangle.NormalCCW());
}

bool ModuleScene::RayCastMesh(GameObject* gameObject, Mesh * mesh, const LineSegment & worldSpaceLineSegment, RayCastHit& rayCastHit) const
{
	bool hit = false;
	LineSegment localGameObjectSpaceLineSegment = ProjectLineSegmentToGameObjectsLocalSpace(worldSpaceLineSegment, *gameObject);
	Triangle triangle;
	size_t index = 0;
	while (NextMeshTriangle(mesh, triangle, index))
	{
		float3 point;
		float normalizedDistance;
		if (localGameObjectSpaceLineSegment.Intersects(triangle, &normalizedDistance, &point))
		{
			if (normalizedDistance < rayCastHit.normalizedDistance)
			{
				BuildRayCastHit(rayCastHit, gameObject, point, normalizedDistance, localGameObjectSpaceLineSegment, triangle);
				hit = true;
			}
		}
	}

	return hit;
}

uiState ModuleScene::IsChildOfCanvas(GameObject & g) const
{
	if(App->uiGame->GetCanvas() == nullptr)
	{
		return uiState::NO_CANVAS;
	}

	GameObject* currentGameObject = &g;
	while(currentGameObject != root)
	{
		if(currentGameObject == App->uiGame->GetCanvas()->gameObject)
		{
			return uiState::FATHER_CANVAS;
		}
		currentGameObject = g.GetParent();
	}
	return uiState::NO_FATHER_CANVAS;
}

/// Loads the next triangle adapting triangles to mesh draw type
/// [out] triangle: the triangle
/// [in] index: the index of the triangle to return
/// [out] index: the next index of the triangle to return
bool NextMeshTriangle(Mesh* mesh, Triangle& triangle, size_t& index)
{
	assert(index <= (size_t)mesh->GetIndicesNumber());

	bool ret = false;
	if (index < (size_t)mesh->GetIndicesNumber()) {
		if (mesh->GetDrawMode() == GL_TRIANGLES)
		{
			assert(index <= (size_t)(mesh->GetIndicesNumber() - 3));
			triangle.a = mesh->GetVertices().at(mesh->GetIndices().at(index++)).position;
			triangle.b = mesh->GetVertices().at(mesh->GetIndices().at(index++)).position;
			triangle.c = mesh->GetVertices().at(mesh->GetIndices().at(index++)).position;
			ret = true;
		}
	}

	return ret;
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
		gameObject->GetParent()->DeleteChild(*gameObject);
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

void ModuleScene::DrawHierarchy(GameObject* gameObject) const
{
	const float3& position = gameObject->GetTransform()->GetWorldPosition();

	float3 parentPosition;

	float4x4 matrixTransposedParent = gameObject->GetTransform()->GetModelMatrix().Transposed();
	float* objectMatrixParent = (float*)matrixTransposedParent.ptr();
	ImGuizmo::DecomposeMatrixToComponents(objectMatrixParent, parentPosition.ptr(), float3().ptr(), float3().ptr());

	for(GameObject* child : gameObject->GetChildren())
	{
		float3 childPosition;

		float4x4 matrixTransposed = child->GetTransform()->GetModelMatrix().Transposed();
		float* objectMatrix = (float*)matrixTransposed.ptr();
		ImGuizmo::DecomposeMatrixToComponents(objectMatrix, childPosition.ptr(), float3().ptr(), float3().ptr());

		float lineWidth;

		glGetFloatv(GL_LINE_WIDTH, &lineWidth);

		glLineWidth(3.0f);

		glColor3f(255.0f, 255.0f, 0.0f);

		glBegin(GL_LINES);

		glVertex3f(parentPosition.x, parentPosition.y, parentPosition.z);
		glVertex3f(childPosition.x, childPosition.y, childPosition.z);

		glEnd();

		glLineWidth(lineWidth);

		DrawHierarchy(child);
	}
}

void ModuleScene::SaveRecursive(GameObject* gameObject, nlohmann::json& data) const
{
	if(gameObject != root)
	{
		json jsonGameObject;
		gameObject->Save(jsonGameObject);

		data.push_back(jsonGameObject);
	}

	for(GameObject* child : gameObject->GetChildren())
	{
		SaveRecursive(child, data);
	}
}

void ModuleScene::LoadScene(const nlohmann::json& jsonScene)
{
	CleanUp();

	App->uiGame->ClearCanvas();

	Init();

	for(const json& jsonGameObject : jsonScene["gameObjects"])
	{
		AddEmptyEmpty(*root)->PreLoad(jsonGameObject);
	}

	for(const json& jsonGameObject : jsonScene["gameObjects"])
	{
		Get(jsonGameObject["uuid"].get<ENGINE_UUID>().id)->Load(jsonGameObject);
	}
}

void ModuleScene::SaveScene(nlohmann::json& jsonScene) const
{
	jsonScene.clear();

	json jsonGameObjects;
	SaveRecursive(root, jsonGameObjects);

	jsonScene["gameObjects"] = jsonGameObjects;

	// more scene stuff ...
}

int32_t ModuleScene_InstantiateCubeInternal()
{
	return reinterpret_cast<int32_t>(App->scene->AddCube(*App->scene->GetRoot()));
}