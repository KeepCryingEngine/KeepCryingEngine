#ifndef _QUADTREENODE_H_
#define _QUADTREENODE_H_

#include <AABB.h>
#include <vector>

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode();
	virtual ~QuadtreeNode();

	void Create(const AABB& limits);

	void Insert(GameObject* gameObject);

private:
	void Add(GameObject* gameObject);
	void Divide();

private:
	const static int bucketSize = 5;
	
	AABB aabb;
	QuadtreeNode* children = nullptr;
	std::vector<GameObject*> content;
};

#endif