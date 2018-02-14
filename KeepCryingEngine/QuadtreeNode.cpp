#include "QuadtreeNode.h"

#include <float2.h>

QuadtreeNode::QuadtreeNode()
{ }

QuadtreeNode::~QuadtreeNode()
{ }

uint QuadtreeNode::GetChildrenAmount() const
{
	return 4;
}

void QuadtreeNode::Divide(AABB* aabbs)
{
	float2 bL = aabb.minPoint.xz();
	float2 tR = aabb.maxPoint.xz();

	float minX = bL.x;
	float minZ = bL.y;
	float maxX = tR.x;
	float maxZ = tR.y;
	float midX = 0.5f * (minX + maxX);
	float midZ = 0.5f * (minZ + maxZ);

	float minY = aabb.minPoint.y;
	float maxY = aabb.maxPoint.y;

	aabbs[0] = AABB(float3(minX, minY, midZ), float3(midX, maxY, maxZ));
	aabbs[1] = AABB(float3(midX, minY, midZ), float3(maxX, maxY, maxZ));
	aabbs[2] = AABB(float3(minX, minY, minZ), float3(midX, maxY, midZ));
	aabbs[3] = AABB(float3(midX, minY, minZ), float3(maxX, maxY, midZ));
}

TreeNode* QuadtreeNode::CreateChildren() const
{
	return new QuadtreeNode[GetChildrenAmount()];
}

/* float QuadtreeNode::GetMinDrawY() const
{
	return -5.0f;
}

float QuadtreeNode::GetMaxDrawY() const
{
	return 5.0f;
} */