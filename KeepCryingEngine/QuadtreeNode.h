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

	/* template<typename TYPE>
	void Intersect(std::vector<GameObject*>& gameObjects, const TYPE& primitive) const
	{
		if(aabb2D.Intersects(primitive))
		{
			for(GameObject* candidate : content)
			{
				AABB candidateAABB = candidate->GetAABB();
				AABB2D candidateAABB2D(candidateAABB.minPoint.xz(), candidateAABB.maxPoint.xz());

				if(candidateAABB2D.Intersects(primitive))
				{
					gameObjects.push_back(candidate);
				}
			}

			if(children != nullptr)
			{
				for(size_t i = 0; i < 4; ++i)
				{
					children[i].Intersect(gameObjects, primitive);
				}
			}
		}
	} */

	void Print(uint level = 0) const;

	void Draw() const;

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