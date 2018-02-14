#ifndef _OCTREENODE_H_
#define _OCTREENODE_H_

#include "TreeNode.h"

class GameObject;

class OctreeNode : public TreeNode
{
public:
	OctreeNode();

	virtual ~OctreeNode();

protected:
	virtual uint GetChildrenAmount() const override;

	virtual void Divide(AABB* aabbs) override;

	virtual TreeNode* CreateChildren() const override;
};

#endif