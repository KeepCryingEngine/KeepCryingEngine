#include "Quadtree.h"

#include "QuadtreeNode.h"

Quadtree::Quadtree()
{ }

Quadtree::~Quadtree()
{ }

void Quadtree::Create(const AABB& limits)
{
	root = new QuadtreeNode();
	root->Create(limits);
}

void Quadtree::Insert(GameObject* gameObject)
{
	root->Insert(gameObject);
}