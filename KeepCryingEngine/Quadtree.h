#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include <AABB.h>

#include "QuadtreeNode.h"

class GameObject;

class Quadtree
{
public:
	Quadtree();
	virtual ~Quadtree();

	void Create(const AABB& aabb);

	void Clear();

	void Insert(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	/* template<typename TYPE>
	void Intersect(std::vector<GameObject*>& gameObjects, const TYPE& primitive) const
	{
		if(root)
		{
			root->Intersect(gameObjects, primitive);
		}
	} */

	void Print() const;

	void Draw() const;

private:
	QuadtreeNode* root = nullptr;
};

#endif