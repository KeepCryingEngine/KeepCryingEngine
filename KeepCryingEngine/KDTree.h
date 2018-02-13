#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <AABB.h>
#include <vector>

class GameObject;
class KDtreeNode;

class KDtree
{
public:
	KDtree();
	virtual ~KDtree();

	void Create(const AABB& aabb);

	void Clear();

	void Insert(GameObject* gameObject);

	void Remove(GameObject* gameObject);

	void Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const;

	void Print() const;

	void Draw() const;

private:
	KDtreeNode * root = nullptr;
};

#endif