#ifndef _KDTREE_H_
#define _KDTREE_H_

#include "Tree.h"

class KDtree : public Tree
{
public:
	KDtree();

	virtual ~KDtree();

protected:
	virtual TreeNode* CreateRoot() const override;
};

#endif