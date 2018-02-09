#include "Quadtree.h"

#include "Globals.h"
#include "QuadtreeNode.h"

Quadtree::Quadtree()
{ }

Quadtree::~Quadtree()
{ }

void Quadtree::Create(const AABB2D& limits)
{
	root = new QuadtreeNode();
	root->Create(limits);
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