#ifndef _TREENODE_H_
#define _TREENODE_H_

#include <vector>
#include <AABB.h>

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

	void Print(uint level = 0) const;

	void Draw() const;

	const AABB& GetAABB() const;

	const std::vector<GameObject*>& GetContent() const;

	void GetAllContent(std::vector<GameObject*>& allContent) const;

protected:
	virtual uint GetChildrenAmount() const = 0;

	virtual void Divide(AABB* aabbs) = 0;

	virtual bool EndRecursion(AABB* aabbs) const;

	virtual bool CreateChildren() const = 0;

	virtual float GetMinDrawY() const;

	virtual float GetMaxDrawY() const;

	void Add(GameObject* gameObject);

	void DivideAndReorganizeContent();

	static uint GetNumberIntersections(const AABB& aabb, const std::vector<GameObject*>& content);

protected:
	const static int bucketSize = 5;

	AABB aabb;
	TreeNode* children = nullptr;
	std::vector<GameObject*> content;
};

#endif