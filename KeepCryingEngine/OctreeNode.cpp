#include "OctreeNode.h"

OctreeNode::OctreeNode()
{ }

OctreeNode::~OctreeNode()
{ }

uint OctreeNode::GetChildrenAmount() const
{
	return 8;
}

void OctreeNode::Divide(AABB* aabbs)
{
	float3 minPoint = aabb.minPoint;
	float3 maxPoint = aabb.maxPoint;
	float3 center = aabb.CenterPoint();

	float minX = minPoint.x;
	float minY = minPoint.y;
	float minZ = minPoint.z;

	float maxX = maxPoint.x;
	float maxY = maxPoint.y;
	float maxZ = maxPoint.z;

	float midX = center.x;
	float midY = center.y;
	float midZ = center.z;

	float3 aaa = minPoint;
	float3 bbb = center;
	float3 aba(minX, midY, minZ);
	float3 bcb(midX, maxY, midZ);
	float3 baa(midX, minY, minZ);
	float3 cbb(maxX, midY, midZ);
	float3 bba(midX, midY, minZ);
	float3 ccb(maxX, maxY, midZ);
	float3 aab(minX, minY, midZ);
	float3 bbc(midX, midY, maxZ);
	float3 abb(minX, midY, midZ);
	float3 bcc(midX, maxY, maxZ);
	float3 bab(midX, minY, midZ);
	float3 cbc(maxX, midY, maxZ);
	float3 ccc(maxX, maxY, maxZ);

	aabbs[0] = AABB(aaa, bbb);
	aabbs[1] = AABB(aba, bcb);
	aabbs[2] = AABB(baa, cbb);
	aabbs[3] = AABB(bba, ccb);
	aabbs[4] = AABB(aab, bbc);
	aabbs[5] = AABB(abb, bcc);
	aabbs[6] = AABB(bab, cbc);
	aabbs[7] = AABB(bbb, ccc);
}

TreeNode* OctreeNode::CreateChildren() const
{
	return new OctreeNode[GetChildrenAmount()];
}