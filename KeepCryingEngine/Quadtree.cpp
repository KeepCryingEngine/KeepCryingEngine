#include "Quadtree.h"

#include "QuadtreeNode.h"

Quadtree::Quadtree()
{ }

Quadtree::~Quadtree()
{ }

void Quadtree::Create(const AABB& aabb)
{
	root = new QuadtreeNode();
	root->Create(aabb);
}

void Quadtree::Clear()
{
	if(root != nullptr)
	{
		root->Clear();

		RELEASE(root);
	}
}

void Quadtree::Insert(GameObject* gameObject)
{
	if(root != nullptr)
	{
		root->Insert(gameObject);
	}
}

void Quadtree::Remove(GameObject* gameObject)
{
	if(root != nullptr)
	{
		root->Remove(gameObject);
	}
}

void Quadtree::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
{
	if(root)
	{
		root->Intersect(gameObjects, frustum);
	}
}

void Quadtree::Print() const
{
	if(root != nullptr)
	{
		root->Print();
	}
}

void Quadtree::Draw() const
{
	if(root != nullptr)
	{
		root->Draw();
	}
}