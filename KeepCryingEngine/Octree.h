#ifndef _OCTREE_H_
#define _OCTREE_H_

#include <AABB.h>
#include <vector>

class GameObject;
class OctreeNode;

class Octree
{
public:
	Octree();
	virtual ~Octree();

	void Create(const AABB& aabb);

	void Clear();

	void Insert(GameObject* gameObject);

	void Remove(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	void Print() const;

	void Draw() const;

private:
	OctreeNode* root = nullptr;
};

#endif