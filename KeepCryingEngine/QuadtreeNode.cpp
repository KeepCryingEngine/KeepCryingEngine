#include "QuadtreeNode.h"

#include <Frustum.h>

#include "Camera.h"
#include "GameObject.h"

using namespace std;

QuadtreeNode::QuadtreeNode()
{ }

QuadtreeNode::~QuadtreeNode()
{ }

void QuadtreeNode::Create(const AABB& aabb)
{
	this->aabb = aabb;
}

void QuadtreeNode::Clear()
{
	content.clear();

	if(children != nullptr)
	{
		for(size_t i = 0; i < 4; ++i)
		{
			children[i].Clear();
		}

		RELEASE_ARRAY(children);
	}
}

void QuadtreeNode::Insert(GameObject* gameObject)
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

void QuadtreeNode::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
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
			for(size_t i = 0; i < 4; ++i)
			{
				children[i].Intersect(gameObjects, frustum);
			}
		}
	}
}

void QuadtreeNode::Print(uint level) const
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
		for(size_t i = 0; i < 4; ++i)
		{
			children[i].Print(level + 1);
		}
	}
}

#include <GL/glew.h>

void QuadtreeNode::Draw() const
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

	float minY = -5.0f;
	float maxY = 5.0f;

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
		for(size_t i = 0; i < 4; ++i)
		{
			children[i].Draw();
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

void QuadtreeNode::DivideAndReorganizeContent()
{
	float2 bL = aabb.minPoint.xz();
	float2 tR = aabb.maxPoint.xz();

	float minX = bL.x;
	float minZ = bL.y;
	float maxX = tR.x;
	float maxZ = tR.y;
	float midX = 0.5f * (minX + maxX);
	float midZ = 0.5f * (minZ + maxZ);

	AABB topLeft(float3(minX, -1011, midZ), float3(midX, 1011, maxZ));
	AABB topRight(float3(midX, -1011, midZ), float3(maxX, 1011, maxZ));
	AABB bottomLeft(float3(minX, -1011, minZ), float3(midX, 1011, midZ));
	AABB bottomRight(float3(midX, -1011, minZ), float3(maxX, 1011, midZ));

	AABB aabbs[4] = { topLeft, topRight, bottomLeft, bottomRight };

	for(size_t i = 0; i < 4; ++i)
	{
		if(GetNumberIntersections(aabbs[i], content) == content.size())
		{
			return;
		}
	}

	children = new QuadtreeNode[4];

	for(size_t i = 0; i < 4; ++i)
	{
		children[i].Create(aabbs[i]);
	}

	for(GameObject* gameObject : content)
	{
		for(size_t i = 0; i < 4; ++i)
		{
			children[i].Insert(gameObject);
		}
	}

	content.clear();
}

uint QuadtreeNode::GetNumberIntersections(const AABB& aabb, const vector<GameObject*>& content)
{
	uint count = 0;

	for(GameObject* gameObject : content)
	{
		count += aabb.Intersects(gameObject->GetAABB());
	}

	return count;
}
