#include "Octree.h"

#include "OctreeNode.h"

Octree::Octree()
{}

Octree::~Octree()
{}

TreeNode* Octree::CreateRoot() const
{
	return new OctreeNode();
}