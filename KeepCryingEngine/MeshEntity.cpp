#include "MeshEntity.h"



MeshEntity::MeshEntity()
{
}


MeshEntity::~MeshEntity()
{
}

const AABB & MeshEntity::GetAABB() const
{
	return aabb;
}

GLuint MeshEntity::GetVertexBufferId() const
{
	return vertexBufferId;
}

GLuint MeshEntity::GetIndicesBufferId() const
{
	return indicesBufferId;
}

GLuint MeshEntity::GetNormalBufferId() const
{
	return normalBufferId;
}

GLsizei MeshEntity::GetVerticesNumber() const
{
	return verticesNumber;
}
