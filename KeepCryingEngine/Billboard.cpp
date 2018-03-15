#include "Billboard.h"

Billboard::Billboard()
{ }

Billboard::~Billboard()
{ }

void Billboard::SetPosition(const float3& position)
{
	this->position = position;
}

const float3& Billboard::GetPosition() const
{
	return position;
}

void Billboard::SetSize(const float2& size)
{
	this->size = size;
}

const float2& Billboard::GetSize() const
{
	return size;
}

void Billboard::ComputeQuad(const Camera& camera, Vertex* outVertices) const
{

}