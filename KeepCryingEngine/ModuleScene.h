#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include <list>
#include <json.hpp>

#include "Module.h"
#include "Octree.h"
#include "Quadtree.h"
#include "KDTree.h"

class GameObject;
class Mesh;

struct RayCastHit 
{
	//The gameObject that was hit
	GameObject* gameObject;
	//The impact point in world space where the ray hit the collider.
	float3 point;
	//The normal of the surface the ray hit.
	float3 normal;
	//The distance from the ray's origin to the impact point.
	float distance;
	//The normalized distance from the ray's origin to the impact point
	float normalizedDistance;
};

enum class uiState
{
	NO_CANVAS,
	NO_FATHER_CANVAS,
	FATHER_CANVAS
};

class ModuleScene : public Module
{
public:
	ModuleScene();
	virtual ~ModuleScene();

	bool Init() override;

	update_status PreUpdate() override;
	update_status Update() override;

	bool CleanUp() override;

	// int GetNewGameObjectId();

	GameObject* GetRoot() const;

	GameObject* Get(int gameObjectId) const;

	GameObject* AddEmpty(GameObject& parent, const char* name = "Empty");

	GameObject* AddCube(GameObject& parent);
	GameObject* AddSphere(GameObject& parent);
	GameObject* AddPlane(GameObject& parent);
	GameObject* AddCamera(GameObject& parent);
	GameObject* AddCanvas(GameObject& parent);
	GameObject* AddImage(GameObject& parent);
	GameObject* AddButton(GameObject& parent);
	GameObject* AddText(GameObject& parent);
	GameObject* AddInputText(GameObject& parent);

	void Generate(int count, float staticPercentage, float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

	void Destroy(GameObject& gameObject);

	void AddStatic(GameObject* gameObject);
	void RemoveStatic(GameObject* gameObject);

	void SetSpacePartitioningStructure(int spacePartitioningStructure);
	bool RayCast(const float3& origin, const float3& direction, float maxDistance, RayCastHit& rayCastHit) const;
	std::vector<RayCastHit> RayCastAll(const float3& origin, const float3& direction, float maxDistance) const;
	void AddToDinamicGameobjectList(GameObject* gameobject);

	void Restore();
	void LoadFromFile(const char* fileName);

	void Save();
	void SaveToFile(const char* fileName) const;

	AABB ComputeAABB() const;

private:
	GameObject* AddEmptyEmpty(GameObject& parent, const char* name = "EmptyEmpty");

	void InitializeRayCastHit(RayCastHit& rayCastHit) const;
	LineSegment BuildLineSegmentForRayCast(const math::float3 & origin, const math::float3 & direction, float maxDistance) const;
	LineSegment ProjectLineSegmentToGameObjectsLocalSpace(const LineSegment& worldSpaceLineSegment, const GameObject& gameObject) const;
	bool RayCastGameObject(GameObject* gameObject, const LineSegment& lineSegment, RayCastHit& rayCastHit) const;
	bool RayCastMesh(GameObject* gameObject, Mesh* mesh, const LineSegment& lineSegment, RayCastHit& rayCastHit) const;

	uiState IsChildOfCanvas(GameObject& g) const;

	void CheckToDestroy();

	void Update(GameObject* gameObject) const;

	void DestroyAndRelease(GameObject* &gameObject) const;

	// void AddToQuadtreeRecursive(GameObject* gameObject);

	void SetVisibleRecursive(GameObject* gameObject, bool visible) const;

	void SetVisibilityRecursive(GameObject* gameObject) const;

	void DrawHierarchy(GameObject* gameObject) const;

	void SaveRecursive(GameObject* gameObject, nlohmann::json& data) const;

	void LoadScene(const nlohmann::json& jsonScene);

	void SaveScene(nlohmann::json& jsonScene) const;

	AABB AABBLightOrientation(GameObject* gameObject) const;

	void EncloseAABB(GameObject* gameObject, AABB& aabb) const;

private:
	GameObject* root = nullptr;

	std::list<GameObject*> toDestroy;

	int currentGameObjectId = 0;

	std::list<std::pair<GameObject*, std::pair<float3, bool>>> generatedGameObjects;

	nlohmann::json jsonData;

	bool validJsonData = false;

	int spaceStructure = 0; // 0 None, 1 Quadtree, 2 Octree, 3 KDTree

	// Quadtree stuff

	Quadtree qTGameObjects;

	// const float QUADTREE_SIZE = 150.0f;
	// const float QUADTREE_HEIGHT = 1000.0f;

	// Octree stuff

	Octree oTGameObjects;

	// const float OCTREE_SIZE = 150.0f;

	// KDTree stuff

	KDtree kTGameObjects;

	//Dinamic objects

	std::vector<GameObject*> dGameobjects;
};

int32_t ModuleScene_InstantiateCubeInternal();

#endif // !_MODULESCENE_H_