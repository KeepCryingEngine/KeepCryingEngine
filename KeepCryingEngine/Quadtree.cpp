#include "Quadtree.h"

#include "QuadtreeNode.h"
#include "GameObject.h"

Quadtree::Quadtree()
{ }

Quadtree::~Quadtree()
{ }

void Quadtree::CreateRoot()
{
	root = new QuadtreeNode();
}

bool Quadtree::CheckNewMinLimit(float3 minPointA, float3 minPointB) const
{
	return minPointA.x < minPointB.x || minPointA.z < minPointB.z;
}

bool Quadtree::CheckNewMaxLimit(float3 maxPointA, float3 maxPointB) const
{
	return maxPointA.x > maxPointB.x || maxPointA.z > maxPointB.z;
}

bool Quadtree::CheckSameMinLimit(float3 minPointA, float3 minPointB) const
{
	return minPointA.x == minPointB.x || minPointA.z == minPointB.z;
}

bool Quadtree::CheckSameMaxLimit(float3 maxPointA, float3 maxPointB) const
{
	return maxPointA.x == maxPointB.x || maxPointA.z == maxPointB.z;
}