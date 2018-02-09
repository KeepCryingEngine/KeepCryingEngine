#include "QuadtreeNode.h"

#include "Globals.h"
#include "GameObject.h"

using namespace std;

QuadtreeNode::QuadtreeNode()
{ }

QuadtreeNode::~QuadtreeNode()
{ }

void QuadtreeNode::Create(const AABB2D& limits)
{
	aabb2D = limits;
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
	AABB gameObjectAABB = gameObject->GetAABB();
	AABB2D gameObjectAABB2D(gameObjectAABB.minPoint.xz(), gameObjectAABB.maxPoint.xz());

	if(aabb2D.Intersects(gameObjectAABB2D))
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

void QuadtreeNode::Print(uint level) const
{
	string indentSpace = "";

	for(int i = 0; i < level; ++i)
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

	float minX = aabb2D.minPoint.x;
	float minZ = aabb2D.minPoint.y;

	float minY = -5.0f;
	float maxY = 5.0f;

	float maxX = aabb2D.maxPoint.x;
	float maxZ = aabb2D.maxPoint.y;

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
	float2 bL = aabb2D.minPoint;
	float2 tR = aabb2D.maxPoint;

	float minX = bL.x;
	float minZ = bL.y;
	float maxX = tR.x;
	float maxZ = tR.y;
	float midX = 0.5f * (minX + maxX);
	float midZ = 0.5f * (minZ + maxZ);

	float2 bM(midX, minZ);
	float2 bR(maxX, minZ);
	float2 tL(minX, maxZ);
	float2 tM(midX, maxZ);
	float2 mL(minX, midZ);
	float2 mR(maxX, midZ);
	float2 center(midX, midZ);

	AABB2D topLeft(mL, tM);
	AABB2D topRight(center, tR);
	AABB2D bottomLeft(bL, center);
	AABB2D bottomRight(bM, mR);

	AABB2D aabb2Ds[4] = { topLeft, topRight, bottomLeft, bottomRight };

	for(size_t i = 0; i < 4; ++i)
	{
		if(GetNumberIntersections(aabb2Ds[i], content) == content.size())
		{
			return;
		}
	}

	children = new QuadtreeNode[4];

	for(size_t i = 0; i < 4; ++i)
	{
		children[i].Create(aabb2Ds[i]);
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

uint QuadtreeNode::GetNumberIntersections(const AABB2D& aabb2D, const vector<GameObject*>& content)
{
	uint count = 0;

	for(GameObject* gameObject : content)
	{
		AABB gameObjectAABB = gameObject->GetAABB();
		AABB2D gameObjectAABB2D(gameObjectAABB.minPoint.xz(), gameObjectAABB.maxPoint.xz());

		count += aabb2D.Intersects(gameObjectAABB2D);
	}

	return count;
}
