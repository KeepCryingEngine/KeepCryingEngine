#include "Quadtree.h"

#include "QuadtreeNode.h"

Quadtree::Quadtree()
{ }

Quadtree::~Quadtree()
{ }

TreeNode* Quadtree::CreateRoot() const
{
	return new QuadtreeNode();
}