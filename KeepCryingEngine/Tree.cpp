#include "Tree.h"

#include "TreeNode.h"
#include "GameObject.h"

using namespace std;

Tree::Tree()
{ }

Tree::~Tree()
{ }

void Tree::Clear()
{
	if(root != nullptr)
	{
		root->Clear();

		RELEASE(root);
	}
}

void Tree::Insert(GameObject* gameObject)
{
	if(root != nullptr)
	{
		float3 g1Min = gameObject->GetAABB().minPoint;
		float3 g1Max = gameObject->GetAABB().maxPoint;

		float3 g2Min = root->GetAABB().minPoint;
		float3 g2Max = root->GetAABB().maxPoint;

		float3 min = g2Min;
		float3 max = g2Max;

		bool resize = false;

		if(CheckNewMinLimit(g1Min, g2Min))
		{
			min = Min(g1Min, g2Min);
			resize = true;
		}

		if(CheckNewMaxLimit(g1Max, g2Max))
		{
			max = Max(g1Max, g2Max);
			resize = true;
		}

		if(resize)
		{
			Resize(AABB(min, max));
		}

		root->Insert(gameObject);
	}
	else
	{
		CreateRoot();
		root->Insert(gameObject);
	}
}

void Tree::Remove(GameObject* gameObject)
{
	if(root != nullptr)
	{
		float3 g1Min = gameObject->GetAABB().minPoint;
		float3 g1Max = gameObject->GetAABB().maxPoint;

		float3 g2Min = root->GetAABB().minPoint;
		float3 g2Max = root->GetAABB().maxPoint;

		root->Remove(gameObject);

		bool rebuild = CheckSameMinLimit(g1Min, g2Min) || CheckSameMaxLimit(g1Max, g2Max);

		if(rebuild)
		{
			Rebuild();
		}
	}
}

void Tree::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
{
	if(root)
	{
		root->Intersect(gameObjects, frustum);
	}
}

void Tree::Print() const
{
	if(root != nullptr)
	{
		root->Print();
	}
}

void Tree::Draw() const
{
	if(root != nullptr)
	{
		root->Draw();
	}
}

void Tree::Rebuild()
{
	vector<GameObject*> allContent;
	root->GetAllContent(allContent);

	Clear();

	for(GameObject* gOContent : allContent)
	{
		Insert(gOContent);
	}
}

void Tree::Resize(const AABB& aabb)
{
	if(root != nullptr)
	{
		vector<GameObject*> allContent;
		root->GetAllContent(allContent);

		root->Clear();
		root->Resize(aabb);

		for(GameObject* gOContent : allContent)
		{
			root->Insert(gOContent);
		}
	}
}