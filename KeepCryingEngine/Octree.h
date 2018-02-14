#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Tree.h"

class Octree : public Tree
{
public:
	Octree();

	virtual ~Octree();

protected:
	virtual TreeNode* CreateRoot() const override;
};

#endif