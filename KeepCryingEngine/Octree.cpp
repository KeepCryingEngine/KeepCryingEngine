#include "Octree.h"

#include "OctreeNode.h"
#include "GameObject.h"

Octree::Octree()
{}

Octree::~Octree()
{}

void Octree::Create(const AABB& aabb)
{
	root = new OctreeNode();
	root->Create(aabb);
}

void Octree::Clear()
{
	if(root != nullptr)
	{
		root->Clear();

		RELEASE(root);
	}
}

void Octree::Insert(GameObject* gameObject)
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
			min = Min(g1Min,g2Min);
			changed = true;
		}
		if(g1Max.x > g2Max.x || g1Max.y > g2Max.y || g1Max.z > g2Max.z)
		{
			max= Max(g1Max, g2Max);
			changed = true;
		}
		if(changed)
		{
			OctreeNode* temp = root;
			root = new OctreeNode();
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
		root = new OctreeNode();
		root->Create(AABB(g1Min, g1Max));
		root->Insert(gameObject);
	}
}

void Octree::Remove(GameObject* gameObject)
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
			OctreeNode* temp = root;
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
			OctreeNode* temp = root;
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

void Octree::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
{
	if(root)
	{
		root->Intersect(gameObjects, frustum);
	}
}

void Octree::Print() const
{
	if(root != nullptr)
	{
		root->Print();
	}
}

void Octree::Draw() const
{
	if(root != nullptr)
	{
		root->Draw();
	}
}