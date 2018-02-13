#include "KDtree.h"

#include "KDtreeNode.h"
#include "GameObject.h"

KDtree::KDtree()
{}

KDtree::~KDtree()
{}

void KDtree::Create(const AABB& aabb)
{
	root = new KDtreeNode();
	root->Create(aabb);
}

void KDtree::Clear()
{
	if(root != nullptr)
	{
		root->Clear();

		RELEASE(root);
	}
}

void KDtree::Insert(GameObject* gameObject)
{
	float3 g1Min = gameObject->GetAABB().minPoint;
	float3 g1Max = gameObject->GetAABB().maxPoint;
	if(root != nullptr)
	{
		float3 g2Min = root->GetAABB().minPoint;
		float3 g2Max = root->GetAABB().maxPoint;
		float3 min = g2Min;
		float3 max = g2Max;
		bool changed = false;

		//Check if new item is in aabb
		if(g1Min.x < g2Min.x || g1Min.y < g2Min.y || g1Min.z < g2Min.z)
		{
			min = Min(g1Min, g2Min);
			changed = true;
		}
		if(g1Max.x > g2Max.x || g1Max.y > g2Max.y || g1Max.z > g2Max.z)
		{
			max = Max(g1Max, g2Max);
			changed = true;
		}
		if(changed)
		{
			KDtreeNode* temp = root;
			root = new KDtreeNode();
			root->Create(AABB(min, max));
			for(GameObject* o : temp->GetContent())
			{
				root->Insert(o);
			}
		}
		root->Insert(gameObject);
	}
	else
	{
		root = new KDtreeNode();
		root->Create(AABB(g1Min, g1Max));
		root->Insert(gameObject);
	}
}

void KDtree::Remove(GameObject* gameObject)
{
	float3 g1Min = gameObject->GetAABB().minPoint;
	float3 g1Max = gameObject->GetAABB().maxPoint;
	if(root != nullptr)
	{
		float3 g2Min = root->GetAABB().minPoint;
		float3 g2Max = root->GetAABB().maxPoint;

		//Check if new item is one of the aabb limits
		if(g1Min.x == g2Min.x || g1Min.y == g2Min.y || g1Min.z == g2Min.z)
		{
			KDtreeNode* temp = root;
			root->Remove(gameObject);
			root = nullptr;
			if(temp->GetContent().size() != 0)
			{
				for(GameObject* o : temp->GetContent())
				{
					Insert(o);
				}
			}
		}
		else if(g1Max.x == g2Max.x || g1Max.y == g2Max.y || g1Max.z == g2Max.z)
		{
			KDtreeNode* temp = root;
			root->Remove(gameObject);
			root = nullptr;
			for(GameObject* o : temp->GetContent())
			{
				Insert(o);
			}
		}
		else
		{
			root->Remove(gameObject);
		}
	}
}

void KDtree::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
{
	if(root)
	{
		root->Intersect(gameObjects, frustum);
	}
}

void KDtree::Print() const
{
	if(root != nullptr)
	{
		root->Print();
	}
}

void KDtree::Draw() const
{
	if(root != nullptr)
	{
		root->Draw();
	}
}