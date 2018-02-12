#ifndef _QUADTREENODE_H_
#define _QUADTREENODE_H_

#include <vector>
#include <AABB.h>

#include "Globals.h"

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode();
	virtual ~QuadtreeNode();

	void Create(const AABB& aabb);

	void Clear();

	void Insert(GameObject* gameObject);

	void Remove(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	void Print(uint level = 0) const;

	void Draw() const;

	AABB & GetAABB();
	std::vector<GameObject*> GetContent() const;

private:
	void Add(GameObject* gameObject);

	void DivideAndReorganizeContent();

	static uint GetNumberIntersections(const AABB& aabb, const std::vector<GameObject*>& content);

private:
	const static int bucketSize = 5;
	
	AABB aabb;
	QuadtreeNode* children = nullptr;
	std::vector<GameObject*> content;
};

#endif