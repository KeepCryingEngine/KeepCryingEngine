#ifndef _QUADTREENODE_H_
#define _QUADTREENODE_H_

#include <vector>
#include <AABB2D.h>

#include "Globals.h"

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode();
	virtual ~QuadtreeNode();

	void Create(const AABB2D& limits);

	void Clear();

	void Insert(GameObject* gameObject);

	void Print(uint level = 0) const;

	void Draw() const;

private:
	void Add(GameObject* gameObject);

	void DivideAndReorganizeContent();

	static uint GetNumberIntersections(const AABB2D& aabb2D, const std::vector<GameObject*>& content);

private:
	const static int bucketSize = 5;
	
	AABB2D aabb2D;
	QuadtreeNode* children = nullptr;
	std::vector<GameObject*> content;
};

#endif