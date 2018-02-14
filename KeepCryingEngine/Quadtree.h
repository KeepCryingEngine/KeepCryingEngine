#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include "Tree.h"

class Quadtree : public Tree
{
public:
	Quadtree();

	virtual ~Quadtree();

protected:
	virtual TreeNode* CreateRoot() const override;
};

#endif