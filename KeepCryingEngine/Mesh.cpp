#include "Mesh.h"

#include <vector>

#include "Globals.h"

using namespace std;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

const AABB & Mesh::GetAABB() const
{
	return aabb;
}

GLuint Mesh::GetVertexBufferId() const
{
	return vertexBufferId;
}

GLuint Mesh::GetIndicesBufferId() const
{
	return indicesBufferId;
}

GLsizei Mesh::GetVerticesNumber() const
{
	return nVertices;
}

GLsizei Mesh::GetIndicesNumber() const
{
	return nIndices;
}

GLenum Mesh::GetDrawMode() const
{
	return drawMode;
}

void Mesh::SetMeshData(const vector<Vertex>& vertices, const vector<GLushort>& indices, GLenum drawMode)
{
	GenerateBuffers(vertices, indices);
	CalculateAABBForMesh(vertices);
	this->drawMode = drawMode;
}

void Mesh::GenerateBuffers(const vector<Vertex> vertices, const vector<GLushort> indices)
{
	assert(vertices.size() > 0);
	assert(indices.size() > 0);

	nVertices = vertices.size();
	nIndices = indices.size();

	//Generate Vertex buffer
	const Vertex * verticesPointer = &vertices[0];
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nVertices, verticesPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Indices buffer
	const GLushort* indicesPointer = &indices[0];
	glGenBuffers(1, &indicesBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * nIndices, indicesPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::CalculateAABBForMesh(const vector<Vertex> &vertices)
{
	float3 * positions = new float3[vertices.size()];
	for (size_t i = 0; i < vertices.size(); i++)
	{
		positions[i] = vertices[i].position;
	}

	aabb.SetNegativeInfinity();
	aabb.Enclose(positions, vertices.size());
	RELEASE_ARRAY(positions);
}