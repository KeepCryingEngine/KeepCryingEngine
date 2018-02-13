#ifndef _MODULESCENE_H_
#define _MODULESCENE_H_

#include <list>

#include "Module.h"
#include "Octree.h"
#include "Quadtree.h"
#include "KDTree.h"

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene();
	virtual ~ModuleScene();

	bool Init() override;
	
	update_status PreUpdate(float deltaTimeS, float realDeltaTimeS) override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

	bool CleanUp() override;

	int GetNewGameObjectId();

	GameObject* GetRoot() const;

	GameObject* Get(unsigned long long int gameObjectId) const;

	GameObject* AddEmpty(GameObject& parent, const char* name = "Empty");

	GameObject* AddCube(GameObject& parent);
	GameObject* AddSphere(GameObject& parent);
	GameObject* AddCamera(GameObject& parent);

	void Generate(int count, float staticPercentage, float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

	void Destroy(GameObject& gameObject);

	void AddStatic(GameObject* gameObject);
	void RemoveStatic(GameObject* gameObject);

	void SetSpacePartitioningStructure(int spacePartitioningStructure);

private:
	void CheckToDestroy();

	void Update(GameObject* gameObject, float deltaTimeS, float realDeltaTimeS) const;

	void DestroyAndRelease(GameObject* &gameObject) const;

	// void AddToQuadtreeRecursive(GameObject* gameObject);

	void SetVisibleRecursive(GameObject* gameObject, bool visible) const;

	void SetVisibilityRecursive(GameObject* gameObject) const;

private:
	GameObject* root = nullptr;

	std::list<GameObject*> toDestroy;

	int currentGameObjectId = 0;

	std::list<std::pair<GameObject*, std::pair<float3, bool>>> generatedGameObjects;

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
};

#endif // !_MODULESCENE_H_