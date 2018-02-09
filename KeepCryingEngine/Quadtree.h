#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include <AABB2D.h>

class GameObject;
class QuadtreeNode;

class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();

	void Create(const AABB2D& limits);

	void Clear();

	void Insert(GameObject* gameObject);

	// void Intersect(std::vector<GameObject*>& gameObjects, PRIMITIVE) const;

	void Print() const;

	void Draw() const;

private:
	QuadtreeNode* root = nullptr;
};

#endif