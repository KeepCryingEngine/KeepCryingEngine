#ifndef _OCTREENODE_H_
#define _OCTREENODE_H_

#include "TreeNode.h"

class OctreeNode : public TreeNode
{
public:
	OctreeNode();

	virtual ~OctreeNode();

protected:
	virtual uint GetChildrenAmount() const override;

	virtual void Divide(AABB* aabbs) override;

	virtual void CreateChildren() override;
};

#endif