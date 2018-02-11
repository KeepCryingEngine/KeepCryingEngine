#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include <AABB.h>
#include <vector>

class GameObject;
class QuadtreeNode;

class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();

	void Create(const AABB& aabb);

	void Clear();

	void Insert(GameObject* gameObject);

	void Remove(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	void Print() const;

	void Draw() const;

private:
	QuadtreeNode* root = nullptr;
};

#endif