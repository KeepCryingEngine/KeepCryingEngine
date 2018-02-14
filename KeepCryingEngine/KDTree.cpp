#include "KDtree.h"

#include "KDtreeNode.h"

KDtree::KDtree()
{ }

KDtree::~KDtree()
{ }

TreeNode* KDtree::CreateRoot() const
{
	return new KDtreeNode();
}