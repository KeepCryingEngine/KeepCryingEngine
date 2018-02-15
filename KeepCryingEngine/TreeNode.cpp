#include "TreeNode.h"

#include <GL/glew.h>
#include <algorithm>

#include "Camera.h"
#include "GameObject.h"

using namespace std;

TreeNode::TreeNode()
{
	aabb.SetNegativeInfinity();
}

TreeNode::~TreeNode()
{ }

void TreeNode::Resize(const AABB& aabb)
{
	this->aabb = aabb;
}

void TreeNode::Clear()
{
	aabb.SetNegativeInfinity();

	content.clear();

	if(!IsLeaf())
	{
		for(size_t i = 0; i < GetChildrenAmount(); ++i)
		{
			children[i]->Clear();

			RELEASE(children[i]);
		}

		children.clear();
	}
}

void TreeNode::Insert(GameObject* gameObject)
{
	if(aabb.Volume() == -inf)
	{
		Resize(gameObject->GetAABB());
	}

	if(aabb.Intersects(gameObject->GetAABB()))
	{
		if(IsLeaf())
		{
			content.push_back(gameObject);

			if(content.size() > bucketSize)
			{
				DivideAndReorganizeContent();
			}
		}
		else
		{
			Add(gameObject);
		}
	}
}

void TreeNode::Remove(GameObject* gameObject)
{
	if(aabb.Intersects(gameObject->GetAABB()))
	{
		if(IsLeaf())
		{
			content.erase(remove(content.begin(), content.end(), gameObject), content.end());
		}
		else
		{
			bool allLeaves = true;
			int countContentChildren = 0;

			for(size_t i = 0; i < GetChildrenAmount(); ++i)
			{
				children[i]->Remove(gameObject);

				allLeaves &= children[i]->IsLeaf();
				countContentChildren += children[i]->content.size();
			}

			if(allLeaves && countContentChildren <= bucketSize)
			{
				for(size_t i = 0; i < GetChildrenAmount(); ++i)
				{
					content.insert(content.end(), children[i]->content.begin(), children[i]->content.end());

					RELEASE(children[i]);
				}

				children.clear();
			}
		}
	}
}

void TreeNode::Intersect(vector<GameObject*>& gameObjects, const Frustum& frustum) const
{
	if(Camera::Intersects(frustum, aabb))
	{
		for(GameObject* candidate : content)
		{
			if(Camera::Intersects(frustum, candidate->GetAABB()))
			{
				gameObjects.push_back(candidate);
			}
		}

		if(!IsLeaf())
		{
			for(size_t i = 0; i < GetChildrenAmount(); ++i)
			{
				children[i]->Intersect(gameObjects, frustum);
			}
		}
	}
}

void TreeNode::Intersect(vector<GameObject*>& gameObjects, const LineSegment& lineSegment) const
{
	if(aabb.Intersects(lineSegment))
	{
		for(GameObject* candidate : content)
		{
			if(candidate->GetAABB().Intersects(lineSegment))
			{
				gameObjects.push_back(candidate);
			}
		}

		if(!IsLeaf())
		{
			for(size_t i = 0; i < GetChildrenAmount(); ++i)
			{
				children[i]->Intersect(gameObjects, lineSegment);
			}
		}
	}
}

void TreeNode::Print(uint level) const
{
	string indentSpace = "";

	for(uint i = 0; i < level; ++i)
	{
		indentSpace += "  ";
	}

	string node = indentSpace;

	node += "Id: %i, Content size: %u";

	LOG_DEBUG(node.c_str(), this, content.size());

	if(!IsLeaf())
	{
		for(size_t i = 0; i < GetChildrenAmount(); ++i)
		{
			children[i]->Print(level + 1);
		}
	}
}

void TreeNode::Draw() const
{
	glPushMatrix();

	float lineWidth;
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	GLfloat previousColor[4];
	glGetFloatv(GL_CURRENT_COLOR, previousColor);

	glLineWidth(5.0f);

	glBegin(GL_LINES);

	glColor3f(255, 255, 0);

	float minX = aabb.minPoint.x;
	float minZ = aabb.minPoint.z;

	float minY = GetMinDrawY();
	float maxY = GetMaxDrawY();

	float maxX = aabb.maxPoint.x;
	float maxZ = aabb.maxPoint.z;

	glVertex3f(minX, minY, minZ);
	glVertex3f(maxX, minY, minZ);

	glVertex3f(maxX, minY, minZ);
	glVertex3f(maxX, minY, maxZ);

	glVertex3f(maxX, minY, maxZ);
	glVertex3f(minX, minY, maxZ);

	glVertex3f(minX, minY, maxZ);
	glVertex3f(minX, minY, minZ);

	glVertex3f(minX, maxY, minZ);
	glVertex3f(maxX, maxY, minZ);

	glVertex3f(maxX, maxY, minZ);
	glVertex3f(maxX, maxY, maxZ);

	glVertex3f(maxX, maxY, maxZ);
	glVertex3f(minX, maxY, maxZ);

	glVertex3f(minX, maxY, maxZ);
	glVertex3f(minX, maxY, minZ);

	glVertex3f(minX, minY, minZ);
	glVertex3f(minX, maxY, minZ);

	glVertex3f(maxX, minY, minZ);
	glVertex3f(maxX, maxY, minZ);

	glVertex3f(maxX, minY, maxZ);
	glVertex3f(maxX, maxY, maxZ);

	glVertex3f(minX, minY, maxZ);
	glVertex3f(minX, maxY, maxZ);

	glEnd();

	glLineWidth(lineWidth);
	glColor3f(previousColor[0], previousColor[1], previousColor[2]);

	glPopMatrix();

	if(!IsLeaf())
	{
		for(size_t i = 0; i < GetChildrenAmount(); ++i)
		{
			children[i]->Draw();
		}
	}
}

const AABB& TreeNode::GetAABB() const
{
	return aabb;
}

const vector<GameObject*>& TreeNode::GetContent() const
{
	return content;
}

void TreeNode::GetAllContent(set<GameObject*>& allContent) const
{
	allContent.insert(content.begin(), content.end());

	if(!IsLeaf())
	{
		for(size_t i = 0; i < GetChildrenAmount(); ++i)
		{
			children[i]->GetAllContent(allContent);
		}
	}
}

bool TreeNode::IsLeaf() const
{
	return children.empty();
}

void TreeNode::SetUpChildren(const AABB* aabbs) const
{
	for(size_t i = 0; i < GetChildrenAmount(); ++i)
	{
		children[i]->Resize(aabbs[i]);
	}
}

float TreeNode::GetMinDrawY() const
{
	return aabb.minPoint.y;
}

float TreeNode::GetMaxDrawY() const
{
	return aabb.maxPoint.y;
}

bool TreeNode::EndRecursion(AABB* aabbs) const
{
	for(size_t i = 0; i < GetChildrenAmount(); ++i)
	{
		if(GetNumberIntersections(aabbs[i], content) == content.size())
		{
			return true;
		}
	}

	return false;
}

void TreeNode::Add(GameObject* gameObject)
{
	for(size_t i = 0; i < GetChildrenAmount(); ++i)
	{
		children[i]->Insert(gameObject);
	}
}

void TreeNode::DivideAndReorganizeContent()
{
	AABB* aabbs = new AABB[GetChildrenAmount()];

	Divide(aabbs);

	if(EndRecursion(aabbs))
	{
		RELEASE_ARRAY(aabbs);

		return;
	}

	CreateChildren();

	SetUpChildren(aabbs);

	RELEASE_ARRAY(aabbs);

	for(GameObject* gameObject : content)
	{
		for(size_t i = 0; i < GetChildrenAmount(); ++i)
		{
			children[i]->Insert(gameObject);
		}
	}

	content.clear();
}

uint TreeNode::GetNumberIntersections(const AABB& aabb, const vector<GameObject*>& content)
{
	uint count = 0;

	for(GameObject* gameObject : content)
	{
		count += aabb.Intersects(gameObject->GetAABB());
	}

	return count;
}