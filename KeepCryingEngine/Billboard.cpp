#include "Billboard.h"

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

Billboard::Billboard()
{ }

Billboard::~Billboard()
{ }

void Billboard::SetLocalPosition(const float3 & position)
{
	localPosition = position;
}

void Billboard::SetWorldPosition(const float3 & position)
{
	worldPosition = position;
}

const float3 & Billboard::GetWorldPosition() const
{
	return worldPosition;
}

const float3 & Billboard::GetLocalPosition() const
{
	return localPosition;
}

void Billboard::SetSize(const float2& size)
{
	this->size = size;
}

const float2& Billboard::GetSize() const
{
	return size;
}

void Billboard::ComputeQuad(const Camera& camera, std::vector<float3>* outverticesPos) const
{
	float3 n = (camera.GetPosition() - worldPosition).Normalized();
	float3 u = float3::unitY;

	float3 r = n.Cross(u);
	n = r.Cross(u);

	//Calculate vertices pos
	float3 topLeft = localPosition + r * size.x / 2 + u * size.y / 2;
	float3 topRight = localPosition - r * size.x / 2 + u * size.y / 2;
	float3 bottomRight = localPosition - r * size.x / 2 - u * size.y / 2;
	float3 bottomLeft = localPosition + r * size.x / 2 - u * size.y / 2;

	outverticesPos->push_back(bottomLeft);
	outverticesPos->push_back(bottomRight);
	outverticesPos->push_back(topRight);
	outverticesPos->push_back(topLeft);

}

void Billboard::ComputeQuadInitial(const Camera & camera, std::vector<float3>* outverticesPos, std::vector<float2>* outverticesUv, std::vector<GLushort>* indices) const
{
	ComputeQuad(camera,outverticesPos);

	float2 topLeftUv = { 0.0f, 1.0f };
	float2 topRight = { 1.0f, 1.0f };
	float2 bottomRight = { 1.0f, 0.0f };
	float2 bottomLeft = { 0.0f, 0.0f };

	int indicesBase = outverticesUv->size();

	outverticesUv->push_back(bottomLeft);
	outverticesUv->push_back(bottomRight);
	outverticesUv->push_back(topRight);
	outverticesUv->push_back(topLeftUv);


	indices->push_back(indicesBase + 0);
	indices->push_back(indicesBase + 1);
	indices->push_back(indicesBase + 3);
	indices->push_back(indicesBase + 1);
	indices->push_back(indicesBase + 2);
	indices->push_back(indicesBase + 3);
}
