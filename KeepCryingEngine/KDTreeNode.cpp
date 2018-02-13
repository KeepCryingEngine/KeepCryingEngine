#include "KDtreeNode.h"

#include <Frustum.h>
#include <GL/glew.h>
#include <algorithm>


#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

using namespace std;

KDtreeNode::KDtreeNode()
{}

KDtreeNode::~KDtreeNode()
{}

void KDtreeNode::Create(const AABB& aabb, CutAxis cut)
{
	actualCut = cut;
	this->aabb = aabb;
}

void KDtreeNode::Clear()
{
	content.clear();

	if(children != nullptr)
	{
		for(size_t i = 0; i < 2; ++i)
		{
			children[i].Clear();
		}

		RELEASE_ARRAY(children);
	}
}

void KDtreeNode::Insert(GameObject* gameObject)
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

void KDtreeNode::Remove(GameObject* gameObject)
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

			for(size_t i = 0; i < 2; ++i)
			{
				children[i].Remove(gameObject);

				allLeaves &= children[i].children == nullptr;
				countContentChildren += children[i].content.size();
			}

			if(allLeaves && countContentChildren <= bucketSize)
			{
				for(size_t i = 0; i < 2; ++i)
				{
					content.insert(content.end(), children[i].content.begin(), children[i].content.end());
				}

				RELEASE_ARRAY(children);
			}
		}
	}
}

void KDtreeNode::Intersect(std::vector<GameObject*>& gameObjects, const Frustum& frustum) const
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
			for(size_t i = 0; i < 2; ++i)
			{
				children[i].Intersect(gameObjects, frustum);
			}
		}
	}
}

void KDtreeNode::Print(uint level) const
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
		for(size_t i = 0; i < 2; ++i)
		{
			children[i].Print(level + 1);
		}
	}
}

void KDtreeNode::Draw() const
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
		for(size_t i = 0; i < 2; ++i)
		{
			children[i].Draw();
		}
	}
}

AABB & KDtreeNode::GetAABB()
{
	return aabb;
}

std::vector<GameObject*> KDtreeNode::GetContent() const
{
	return content;
}

void KDtreeNode::Add(GameObject* gameObject)
{
	for(size_t i = 0; i < 2; ++i)
	{
		children[i].Insert(gameObject);
	}
}

void KDtreeNode::DivideAndReorganizeContent()
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
	AABB leftAABB(minPoint, leftAABBMax);
	AABB rightAABB(rightAABBMin, maxPoint);


	AABB aabbs[2] = { leftAABB, rightAABB};

	/*for(size_t i = 0; i < 2; ++i)
	{
		if(GetNumberIntersections(aabbs[i], content) == content.size())
		{
			return;
		}
	}*/

	children = new KDtreeNode[2];

	for(size_t i = 0; i < 2; ++i)
	{
		children[i].Create(aabbs[i],GetNextAxis(actualCut));
	}

	for(GameObject* gameObject : content)
	{
		for(size_t i = 0; i < 2; ++i)
		{
			children[i].Insert(gameObject);
		}
	}

	content.clear();
}

uint KDtreeNode::GetNumberIntersections(const AABB& aabb, const vector<GameObject*>& content)
{
	uint count = 0;

	for(GameObject* gameObject : content)
	{
		count += aabb.Intersects(gameObject->GetAABB());
	}

	return count;
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

float KDtreeNode::GetMedianAxis(CutAxis axis)
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
