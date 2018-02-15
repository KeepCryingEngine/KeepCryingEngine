#ifndef _TREE_H_
#define _TREE_H_

#include <AABB.h>
#include <vector>

class TreeNode;
class GameObject;

class Tree
{
public:
	Tree();

	virtual ~Tree();

	void Clear();

	void Insert(GameObject* gameObject);

	void Remove(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	void Intersect(std::vector<GameObject*>& gameObjects, const LineSegment& lineSegment) const;

	void Print() const;

	void Draw() const;

protected:
	virtual TreeNode* CreateRoot() const = 0;

private:
	bool CheckNewMinLimit(float3 minPointA, float3 minPointB) const;

	bool CheckNewMaxLimit(float3 maxPointA, float3 maxPointB) const;

	bool CheckSameMinLimit(float3 minPointA, float3 minPointB) const;

	bool CheckSameMaxLimit(float3 maxPointA, float3 maxPointB) const;

	void Rebuild();

	void Resize(const AABB& aabb);

protected:
	TreeNode* root = nullptr;
};

#endif