#include "QuadtreeNode.h"

#include "GameObject.h"

QuadtreeNode::QuadtreeNode()
{ }

QuadtreeNode::~QuadtreeNode()
{ }

void QuadtreeNode::Create(const AABB& limits)
{
	aabb = limits;
}

void QuadtreeNode::Insert(GameObject* gameObject)
{
	if(aabb.Intersects(gameObject->GetAABB()))
	{
		if(children == nullptr)
		{
			content.push_back(gameObject);

			if(content.size() >= bucketSize)
			{
				Divide();
				Add(gameObject);
			}
		}
		else
		{
			Add(gameObject);
		}
	}
}

void QuadtreeNode::Add(GameObject* gameObject)
{
	for(size_t i = 0; i < 4; ++i)
	{
		children[i].Insert(gameObject);
	}
}

void QuadtreeNode::Divide()

	AABB topRight;
	AABB topLeft;
	AABB bottomRight;
	AABB bottomLeft;

	children = new QuadtreeNode[4];

	children[0].Create(topRight);
	children[1].Create(topLeft);
	children[2].Create(bottomRight);
	children[3].Create(bottomLeft);
}