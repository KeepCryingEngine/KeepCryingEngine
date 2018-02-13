#ifndef _QUADTREE_H_
#define _QUADTREE_H_

#include "Tree.h"

class Quadtree : public Tree
{
public:
	Quadtree();

	virtual ~Quadtree();

protected:
	virtual void CreateRoot() override;

	virtual bool CheckNewMinLimit(float3 minPointA, float3 minPointB) const override;

	virtual bool CheckNewMaxLimit(float3 maxPointA, float3 maxPointB) const override;

	virtual bool CheckSameMinLimit(float3 minPointA, float3 minPointB) const override;

	virtual bool CheckSameMaxLimit(float3 maxPointA, float3 maxPointB) const override;
};

#endif