#include "KDtreeNode.h"

#include <list>
#include <assert.h>

#include "Transform.h"
#include "GameObject.h"

using namespace std;

KDtreeNode::KDtreeNode()
{ }

KDtreeNode::~KDtreeNode()
{ }

void KDtreeNode::SetCutAxis(CutAxis cut)
{
	actualCut = cut;
}

uint KDtreeNode::GetChildrenAmount() const
{
	return 2;
}

void KDtreeNode::Divide(AABB* aabbs)
{
	float3 minPoint = aabb.minPoint;
	float3 maxPoint = aabb.maxPoint;

	float median = GetMedianAxis(actualCut);

	float3 leftAABBMax = maxPoint;
	float3 rightAABBMin = minPoint;

	switch(actualCut)
	{
		case KDtreeNode::CutAxis::X:
		{
			leftAABBMax.x = median;
			rightAABBMin.x = median;
		}
		break;
		case KDtreeNode::CutAxis::Y:
		{
			leftAABBMax.y = median;
			rightAABBMin.y = median;
		}
		break;
		case KDtreeNode::CutAxis::Z:
		{
			leftAABBMax.z = median;
			rightAABBMin.z = median;
		}
		break;
		default:
			break;
	}

	aabbs[0] = AABB(minPoint, leftAABBMax);
	aabbs[1] = AABB(rightAABBMin, maxPoint);
}

TreeNode* KDtreeNode::CreateChildren() const
{
	return new KDtreeNode[GetChildrenAmount()];
}

void KDtreeNode::SetUpChildren(const AABB* aabbs) const
{
	KDtreeNode* children = (KDtreeNode*)this->children;

	for(size_t i = 0; i < GetChildrenAmount(); ++i)
	{
		children[i].Resize(aabbs[i]);
		children[i].SetCutAxis(GetNextAxis(actualCut));
	}
}

KDtreeNode::CutAxis KDtreeNode::GetNextAxis(KDtreeNode::CutAxis actual) const
{
	CutAxis ret;
	switch(actual)
	{
		case KDtreeNode::CutAxis::X:
		{
			ret = KDtreeNode::CutAxis::Y;
		}
			break;
		case KDtreeNode::CutAxis::Y:
		{
			ret = KDtreeNode::CutAxis::Z;
		}
			break;
		case KDtreeNode::CutAxis::Z:
		{
			ret = KDtreeNode::CutAxis::X;
		}
			break;
		default:
			assert(false);
	}
	return ret;
}

float KDtreeNode::GetMedianAxis(CutAxis axis) const
{
	list<float> points;
	switch(actualCut)
	{
		case KDtreeNode::CutAxis::X:
		{
			for(GameObject* g : content)
			{
				points.push_back(g->GetTransform()->GetWorldPosition().x);
			}
		}
			break;
		case KDtreeNode::CutAxis::Y:
		{
			for(GameObject* g : content)
			{
				points.push_back(g->GetTransform()->GetWorldPosition().y);
			}
		}
			break;
		case KDtreeNode::CutAxis::Z:
		{
			for(GameObject* g : content)
			{
				points.push_back(g->GetTransform()->GetWorldPosition().z);
			}
		}
			break;
		default:
			assert(false);
	}

	bool isEven = ((points.size()%2)==0);

	points.sort();

	float ret;
	int higherIndex = (points.size() / 2);
	int lowerIndex = higherIndex - 1;
	if(isEven)
	{
		list<float>::iterator it = next(points.begin(), lowerIndex);
		float v1 = (*it);
		float v2 = *(++it);
		ret = (v1 + v2) / 2.0f;
	}
	else
	{
		ret = *next(points.begin(), lowerIndex);
	}

	return ret;
}