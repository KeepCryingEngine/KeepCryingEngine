#ifndef _QUADTREE_H_
#define _QUADTREE_H_

// #include <vector>
#include <AABB.h>

class GameObject;
class QuadtreeNode;

class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();

	void Create(const AABB& limits);

	// void Clear();

	void Insert(GameObject* gameObject);

	// void Intersect(std::vector<GameObject*>& gameObjects, PRIMITIVE) const;

private:
	QuadtreeNode* root = nullptr;
};

#endif