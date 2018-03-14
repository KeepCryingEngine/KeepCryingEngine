#include "Mesh.h"

#include <vector>

#include "Globals.h"

using namespace std;

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &indicesBufferId);
	glDeleteBuffers(1, &vertexBufferId);
}

void Mesh::SetPath(const std::experimental::filesystem::path & path)
{
	this->path = path;
}

void Mesh::SetName(const std::string & name)
{
	this->name = name;
}

const std::experimental::filesystem::path & Mesh::GetPath() const
{
	return path;
}

const std::string & Mesh::GetName() const
{
	return name;
}

void Mesh::Load(const nlohmann::json & json)
{
}

void Mesh::Save(nlohmann::json & json)
{
	nlohmann::json mesh;

	mesh["name"] = name;
	mesh["path"] = path.string();

	json.push_back(mesh);
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

void Mesh::SetMeshData(const vector<Vertex>& vertices, const vector<GLushort>& indices, const vector<Bone> bones, GLenum drawMode)
{
	GenerateBuffers(vertices, indices);
	CalculateAABBForMesh(vertices);
	this->drawMode = drawMode;
	this->vertices = vertices;
	this->originalVertices = vertices;
	this->indices = indices;
	this->bones = bones;
}

const std::vector<Vertex>& Mesh::GetVertices() const
{
	return vertices;
}

const std::vector<GLushort>& Mesh::GetIndices() const
{
	return indices;
}

const std::vector<Vertex>& Mesh::GetOriginalVertices() const
{
	return originalVertices;
}

void Mesh::UpdateVertices(const std::vector<Vertex>& vertices)
{
	assert(dynamicDraw);
	assert(vertices.size() == nVertices);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	this->vertices = vertices;
}

const std::vector<Bone>& Mesh::GetBones() const
{
	return bones;
}

void Mesh::SetDynamicDraw(bool dynamicDraw)
{
	if(this->dynamicDraw != dynamicDraw)
	{
		glDeleteBuffers(1, &vertexBufferId);

		const Vertex * verticesPointer = &vertices[0];
		glGenBuffers(1, &vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nVertices, verticesPointer, dynamicDraw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	this->dynamicDraw = dynamicDraw;
}

void Mesh::GenerateBuffers(const vector<Vertex>& vertices, const vector<GLushort>& indices)
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