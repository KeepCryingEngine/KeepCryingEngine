#include "OctreeNode.h"

#include <Frustum.h>
#include <GL/glew.h>
#include <algorithm>

#include "Camera.h"
#include "GameObject.h"

using namespace std;

OctreeNode::OctreeNode()
{}

OctreeNode::~OctreeNode()
{}

void OctreeNode::Create(const AABB& aabb)
{
	this->aabb = aabb;
}

void OctreeNode::Clear()
{
	content.clear();

	if(children != nullptr)
	{
		for(size_t i = 0; i < 8; ++i)
		{
			children[i].Clear();
		}

		RELEASE_ARRAY(children);
	}
}

void OctreeNode::Insert(GameObject* gameObject)
{
	if(aabb.Intersects(gameObject->GetAABB()))
	{
		if(children == nullptr)
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

void OctreeNode::Remove(GameObject* gameObject)
{
	if(aabb.Intersects(gameObject->GetAABB()))
	{
		if(children == nullptr)
		{
			content.erase(remove(content.begin(), content.end(), gameObject), content.end());
		}
		else
		{
			bool allLeaves = true;
			int countContentChildren = 0;

			for(size_t i = 0; i < 8; ++i)
			{
				children[i].Remove(gameObject);

				allLeaves &= children[i].children == nullptr;
				countContentChildren += children[i].content.size();
			}

			if(allLeaves && countContentChildren <= bucketSize)
			{
				for(size_t i = 0; i < 8; ++i)
				{
					content.insert(content.end(), children[i].content.begin(), children[i].content.end());
				}

				RELEASE_ARRAY(children);
			}
		}
	}
}

void OctreeNode::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
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

		if(children != nullptr)
		{
			for(size_t i = 0; i < 8; ++i)
			{
				children[i].Intersect(gameObjects, frustum);
			}
		}
	}
}

void OctreeNode::Print(uint level) const
{
	string indentSpace = "";

	for(uint i = 0; i < level; ++i)
	{
		indentSpace += "  ";
	}

	string node = indentSpace;

	node += "Id: %i, Content size: %u";

	LOG_DEBUG(node.c_str(), this, content.size());

	if(children != nullptr)
	{
		for(size_t i = 0; i < 8; ++i)
		{
			children[i].Print(level + 1);
		}
	}
}

void OctreeNode::Draw() const
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

	float minY = aabb.minPoint.y;
	float maxY = aabb.maxPoint.y;

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

	// ...

	glVertex3f(minX, maxY, minZ);
	glVertex3f(maxX, maxY, minZ);

	glVertex3f(maxX, maxY, minZ);
	glVertex3f(maxX, maxY, maxZ);

	glVertex3f(maxX, maxY, maxZ);
	glVertex3f(minX, maxY, maxZ);

	glVertex3f(minX, maxY, maxZ);
	glVertex3f(minX, maxY, minZ);

	// ...

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

	if(children != nullptr)
	{
		for(size_t i = 0; i < 8; ++i)
		{
			children[i].Draw();
		}
	}
}

AABB & OctreeNode::GetAABB()
{
	return aabb;
}

std::vector<GameObject*> OctreeNode::GetContent() const
{
	return content;
}

void OctreeNode::Add(GameObject* gameObject)
{
	for(size_t i = 0; i < 8; ++i)
	{
		children[i].Insert(gameObject);
	}
}

void OctreeNode::DivideAndReorganizeContent()
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

	AABB aaaAABB(aaa, bbb);
	AABB abaAABB(aba, bcb);
	AABB baaAABB(baa, cbb);
	AABB bbaAABB(bba, ccb);
	AABB aabAABB(aab, bbc);
	AABB abbAABB(abb, bcc);
	AABB babAABB(bab, cbc);
	AABB bbbAABB(bbb, ccc);

	AABB aabbs[8] = { aaaAABB, abaAABB, baaAABB, bbaAABB, aabAABB, abbAABB, babAABB, bbbAABB };

	for(size_t i = 0; i < 8; ++i)
	{
		if(GetNumberIntersections(aabbs[i], content) == content.size())
		{
			return;
		}
	}

	children = new OctreeNode[8];

	for(size_t i = 0; i < 8; ++i)
	{
		children[i].Create(aabbs[i]);
	}

	for(GameObject* gameObject : content)
	{
		for(size_t i = 0; i < 8; ++i)
		{
			children[i].Insert(gameObject);
		}
	}

	content.clear();
}

uint OctreeNode::GetNumberIntersections(const AABB& aabb, const vector<GameObject*>& content)
{
	uint count = 0;

	for(GameObject* gameObject : content)
	{
		count += aabb.Intersects(gameObject->GetAABB());
	}

	return count;
}