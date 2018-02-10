#include "MeshEntity.h"
#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>

#include "Globals.h"

using namespace std;

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

GLsizei MeshEntity::GetVerticesNumber() const
{
	return nVertices;
}

GLsizei MeshEntity::GetIndicesNumber() const
{
	return nIndices;
}

GLenum MeshEntity::GetDrawMode() const
{
	return drawMode;
}

void MeshEntity::SetMeshData(const vector<Vertex>& vertices, const vector<GLushort>& indices, GLenum drawMode)
{
	GenerateBuffers(vertices,indices);
	CalculateAABBForMesh(vertices);
	this->drawMode = drawMode;
}

void MeshEntity::CleanMeshData()
{
}

//void MeshEntity::SetUpCube()
//{
//	drawMode = GL_TRIANGLES;
//	const float size = 1.0f;
//	const size_t nCubeVertices = 24;
//	nVertices = nCubeVertices;
//	float3 positions[nCubeVertices] = {
//		{ -size, -size, -size }, // 0 Front bottom left
//		{ size, -size, -size }, // 1 Front bottom right
//		{ size, size, -size }, // 2 Front top right
//		{ -size, size, -size }, // 3 Front top left
//
//		{ -size, -size, size }, // 4 Left bottom left
//		{ -size, -size, -size }, // 5 Left bottom right
//		{ -size, size, -size }, // 6 Left top right
//		{ -size, size, size }, // 7 Left top left
//
//		{ -size, size, size }, // 8 Back top left
//		{ size, size, size }, // 9 Back top right
//		{ size, -size, size }, // 10 Back bottom right
//		{ -size, -size, size }, // 11 Back bottom left
//
//		{ size, -size, -size }, // 12 Right bottom left
//		{ size, -size, size }, // 13 Right bottom right
//		{ size, size, size }, // 14 Right top right
//		{ size, size, -size }, // 15 Right top left
//
//		{ -size, -size, size }, // 16 Bottom top left
//		{ size, -size, size }, // 17 Bottom top right
//		{ size, -size, -size }, // 18 Bottom bottom right
//		{ -size, -size, -size }, // 19 Bottom bottom left
//
//		{ -size, size, -size }, // 20 Top bottom left
//		{ size, size, -size }, // 2size Top bottom right
//		{ size, size, size }, // 22 Top top right
//		{ -size, size, size } // 23 Top top left
//	};
//	
//	float4 colors[nCubeVertices];
//	for (size_t i = 0; i < nCubeVertices; i++)
//	{
//		colors[i] = float4(100, 100, 100, 100);
//	}
//
//	float2 uv[nCubeVertices] = {
//		{0.0f, 0.0f},
//		{1.0f, 0.0f},
//		{1.0f, 1.0f},
//		{0.0f, 1.0f},
//	
//		{1.0f, 0.0f},
//		{0.0f, 0.0f},
//		{0.0f, 1.0f},
//		{1.0f, 1.0f},
//
//		{1.0f, 1.0f},
//		{0.0f, 1.0f},
//		{0.0f, 0.0f},
//		{1.0f, 0.0f},
//	
//		{0.0f, 0.0f},
//		{1.0f, 0.0f},
//		{1.0f, 1.0f},
//		{0.0f, 1.0f},
//
//		{0.0f, 1.0f},
//		{1.0f, 1.0f},
//		{1.0f, 0.0f},
//		{0.0f, 0.0f},
//
//		{0.0f, 0.0f},
//		{1.0f, 0.0f},
//		{1.0f, 1.0f},
//		{0.0f, 1.0f}
//	};
//
//	const size_t cubeIndicesNumber = 36;
//	nIndices = cubeIndicesNumber;
//	GLushort indicesArray[cubeIndicesNumber] =
//	{
//		3, 1, 0,
//		3, 2, 1,
//
//		7, 5, 4,
//		7, 6, 5,
//
//		11, 9, 8,
//		11, 10, 9,
//
//		15, 13, 12,
//		15, 14, 13,
//
//		19, 17, 16,
//		19, 18, 17,
//
//		23, 21, 20,
//		23, 22, 21
//	};
//
//	float3 normals[nCubeVertices]; // TODO FILL THE NORMALS WITH THE NORMAL VALUE
//	/*float offset = 3;
//	for(size_t i = 0; i < 36; i+=3)
//	{
//	int x1 = indicesArray[i] * offset;
//	int x2 = indicesArray[i + 1] * offset;
//	int x3 = indicesArray[i + 2] * offset;
//
//	int y1 = (indicesArray[i] * offset) + 1;
//	int y2 = (indicesArray[i + 1] * offset) + 1;
//	int y3 = (indicesArray[i + 2] * offset) + 1;
//
//	int z1 = (indicesArray[i] * offset) + 2;
//	int z2 = (indicesArray[i + 1] * offset) + 2;
//	int z3 = (indicesArray[i + 2] * offset) + 2;
//
//	float edge1X = uniqueVertex[x2] - uniqueVertex[x1];
//	float edge1Y = uniqueVertex[y2] - uniqueVertex[y1];
//	float edge1Z = uniqueVertex[z2] - uniqueVertex[z1];
//
//	float edge2X = uniqueVertex[x3] - uniqueVertex[x1];
//	float edge2Y = uniqueVertex[y3] - uniqueVertex[y1];
//	float edge2Z = uniqueVertex[z3] - uniqueVertex[z1];
//
//	float3 vNormal = Cross(float3(edge1X, edge1Y, edge1Z), float3(edge2X, edge2Y, edge2Z)).Normalized();
//	normal[i] = vNormal.x;
//	normal[i + 1] = vNormal.y;
//	normal[i + 2] = vNormal.z;
//	}*/
//
//	Vertex vertices[nCubeVertices];
//	FillVerticesData(vertices, nCubeVertices, positions, normals, colors, uv);
//
//	GenerateBuffers(vertices, nVertices, indicesArray, nIndices);
//
//	CalculateAABBForMesh(positions, nVertices);	
//}

//void MeshEntity::SetUpSphere()
//{
//	drawMode = GL_QUADS;
//	const uint rings = 100;
//	const uint sectors = 100;
//	uint radius = 1;
//
//	float R = 1.0f / (float)(rings - 1);
//	float S = 1.0f / (float)(sectors - 1);
//
//	vector<float3> positions;
//	vector<GLushort> indices;
//	vector<float2> texcoords;
//	vector<float3> normals;
//	
//	const size_t nSphereVertices = rings * sectors;
//	nVertices = nSphereVertices;
//
//	positions.resize(nSphereVertices * 3);
//	texcoords.resize(nSphereVertices * 2);
//	normals.resize(nSphereVertices * 3);
//
//	vector<float3>::iterator v = positions.begin();
//	vector<float2>::iterator t = texcoords.begin();
//	vector<float3>::iterator n = normals.begin();
//	for(unsigned int r = 0; r < rings; r++)
//	{
//		for(unsigned int s = 0; s < sectors; s++)
//		{
//			float y = (float)sin(-M_PI_2 + M_PI * r * R);
//			float x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
//			float z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
//
//			*t++ = float2(s * S, r * R);
//
//			*v++ = float3(x * radius, y * radius, z * radius);
//
//			*n++ = float3(x, y, z);
//		}
//	}
//
//	float4 colors[nSphereVertices];
//	for(size_t i = 0; i < nSphereVertices; i++)
//	{
//		colors[i] = float4(100, 100, 100, 100);
//	}
//
//	indices.resize(rings * sectors * 4);
//	const size_t sphereIndicesNumber = indices.size();
//	nIndices = sphereIndicesNumber;
//
//	vector<GLushort>::iterator i = indices.begin();
//
//	for(unsigned int r = 0; r < rings - 1; r++)
//	{
//		for(unsigned int s = 0; s < sectors - 1; s++)
//		{
//			*i++ = (r + 1) * sectors + s;
//			*i++ = (r + 1) * sectors + s + 1;
//			*i++ = r * sectors + s + 1;
//			*i++ = r * sectors + s;
//		}
//	}
//
//	Vertex vertices[nSphereVertices];
//	FillVerticesData(vertices, nSphereVertices, &positions[0], &normals[0], colors, &texcoords[0]);
//
//	GenerateBuffers(vertices, nVertices, &indices[0], nIndices);
//
//	CalculateAABBForMesh(&positions[0], nVertices);
//}

void MeshEntity::GenerateBuffers(const vector<Vertex> vertices, const vector<GLushort> indices)
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

void MeshEntity::CalculateAABBForMesh(const vector<Vertex> &vertices)
{
	float3 * positions = new float3[vertices.size()];
	for (size_t i = 0; i < vertices.size(); i++)
	{
		positions[i] = vertices[i].position;
	}
	
	aabb.Enclose(positions, vertices.size());
	RELEASE_ARRAY(positions);
}

void MeshEntity::FillVerticesData(Vertex* vertices, GLuint nVertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const
{
	assert(vertices);
	assert(nVertices > 0);
	assert(positions);
	assert(normals);
	assert(colors);
	assert(uvs);
	
	for(GLuint i = 0; i < nVertices; ++i)
	{
		vertices[i].position = positions[i];
		vertices[i].normal = normals[i];
		vertices[i].color = colors[i];
		vertices[i].uv = uvs[i];
	}
}
