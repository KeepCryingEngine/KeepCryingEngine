#ifndef _QUADTREENODE_H_
#define _QUADTREENODE_H_

#include "TreeNode.h"

class GameObject;

class QuadtreeNode : public TreeNode
{
public:
	QuadtreeNode();

	virtual ~QuadtreeNode();
};

#endif