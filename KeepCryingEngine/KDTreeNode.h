#ifndef _KDTREENODE_H_
#define _KDTREENODE_H_

#include "TreeNode.h"

class KDtreeNode : public TreeNode
{
private:
	enum class CutAxis
	{
		X,
		Y,
		Z
	};

public:
	KDtreeNode();

	virtual ~KDtreeNode();

	void SetCutAxis(CutAxis cut);

protected:
	virtual uint GetChildrenAmount() const override;

	virtual void Divide(AABB* aabbs) override;

	virtual void CreateChildren() override;

	virtual void SetUpChildren(const AABB* aabbs) const override;

private:
	CutAxis GetNextAxis(CutAxis actual) const;

	float GetMedianAxis(CutAxis axis) const;

private:
	CutAxis actualCut = CutAxis::X;
};

#endif