#ifndef _TREENODE_H_
#define _TREENODE_H_

#include <set>
#include <AABB.h>
#include <vector>

#include "Globals.h"

class GameObject;

class TreeNode
{
public:
	TreeNode();

	virtual ~TreeNode();

	void Resize(const AABB& aabb);

	void Clear();

	void Insert(GameObject* gameObject);

	void Remove(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	void Intersect(std::vector<GameObject*>& gameObjects, const LineSegment& lineSegment) const;

	void Print(uint level = 0) const;

	void Draw() const;

	const AABB& GetAABB() const;

	const std::vector<GameObject*>& GetContent() const;

	void GetAllContent(std::set<GameObject*>& allContent) const;

	bool IsLeaf() const;

protected:
	virtual uint GetChildrenAmount() const = 0;

	virtual void Divide(AABB* aabbs) = 0;

	virtual void CreateChildren() = 0;

	virtual void SetUpChildren(const AABB* aabbs) const;

	virtual float GetMinDrawY() const;

	virtual float GetMaxDrawY() const;

	bool EndRecursion(AABB* aabbs) const;

	void Add(GameObject* gameObject);

	void DivideAndReorganizeContent();

	static uint GetNumberIntersections(const AABB& aabb, const std::vector<GameObject*>& content);

protected:
	const static int bucketSize = 5;

	AABB aabb;
	std::vector<TreeNode*> children;
	std::vector<GameObject*> content;
};

#endif