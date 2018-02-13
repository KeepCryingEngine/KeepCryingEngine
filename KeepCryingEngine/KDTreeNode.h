#ifndef _KDTREENODE_H_
#define _KDTREENODE_H_

#include <vector>
#include <AABB.h>

#include "Globals.h"

class GameObject;

class KDtreeNode
{
private:
	enum class CutAxis
	{
		X,
		Y,
		Z
	};
public:
	KDtreeNode();
	virtual ~KDtreeNode();

	void Create(const AABB& aabb,CutAxis cut = CutAxis::X);

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

	CutAxis GetNextAxis(CutAxis actual) const;

	float GetMedianAxis(CutAxis axis);

private:
	const static int bucketSize = 1;

	AABB aabb;
	KDtreeNode* children = nullptr;
	std::vector<GameObject*> content;
	CutAxis actualCut;
};

#endif