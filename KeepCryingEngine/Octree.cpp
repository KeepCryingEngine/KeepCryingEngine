#include "Octree.h"

#include "OctreeNode.h"

Octree::Octree()
{}

Octree::~Octree()
{}

void Octree::Create(const AABB& aabb)
{
	root = new OctreeNode();
	root->Create(aabb);
}

void Octree::Clear()
{
	if(root != nullptr)
	{
		root->Clear();

		RELEASE(root);
	}
}

void Octree::Insert(GameObject* gameObject)
{
	if(root != nullptr)
	{
		root->Insert(gameObject);
	}
}

void Octree::Remove(GameObject* gameObject)
{
	if(root != nullptr)
	{
		root->Remove(gameObject);
	}
}

void Octree::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
{
	if(root)
	{
		root->Intersect(gameObjects, frustum);
	}
}

void Octree::Print() const
{
	if(root != nullptr)
	{
		root->Print();
	}
}

void Octree::Draw() const
{
	if(root != nullptr)
	{
		root->Draw();
	}
}