#include "Mesh.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>

#include "GameObject.h"
#include "Application.h"
#include "ModuleRender.h"

using namespace std;

Mesh::Mesh():Component(ComponentType::Mesh)
{
}

Mesh::~Mesh()
{
}

void Mesh::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(changedMode)
	{
		switch(meshMode)
		{
			case MeshMode::CUBE:
			{
				SetUpCube();
			}
			break;

			case MeshMode::SPHERE:
			{
				SetUpSphere();
			}
			break;
		}
		changedMode = false;
	}

	App->renderer->AddToDrawBuffer(*this);
}

void Mesh::DrawUI()
{
	if(ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

		int tmpMeshMode = (int)meshMode;

		if(ImGui::Combo("Type", &tmpMeshMode, "Cube\0Sphere"))
		{
			changedMode = true;
			meshMode = (MeshMode)tmpMeshMode;
		}
	}
}

void Mesh::SetMeshMode(MeshMode mode)
{
	meshMode = mode;
	changedMode = true;
}

vector<ComponentType> Mesh::GetNeededComponents() const
{
	return { ComponentType::Transform };
}

vector<ComponentType> Mesh::GetProhibitedComponents() const
{
	return { ComponentType::Mesh };
}

GLenum Mesh::GetDrawMode()
{
	return drawMode;
}

uint Mesh::GetVertexBufferId() const
{
	return vertexBufferId;
}

uint Mesh::GetIndicesBufferId() const
{
	return indicesBufferId;
}

uint Mesh::GetNormalBufferId() const
{
	return normalBufferId;
}

uint Mesh::GetVerticesNumber() const
{
	return verticesNumber;
}

void Mesh::SetUpCube()
{
	drawMode = GL_TRIANGLES;

	float size = 1.0f;
	float uniqueVertex[8 * 3 * 3] =
	{
		-size, -size, -size, // 0 Front bottom left
		size, -size, -size,  // 1 Front bottom right
		size, size, -size,  // 2 Front top right
		-size, size, -size, // 3 Front top left

		-size, -size, size, // 4 Left bottom left
		-size, -size, -size, // 5 Left bottom right
		-size, size, -size, // 6 Left top right
		-size, size, size,  // 7 Left top left

		-size, size, size,  // 8 Back top left
		size, size, size,  // 9 Back top right
		size, -size, size,  // 10 Back bottom right
		-size, -size, size, // 11 Back bottom left

		size, -size, -size,  // 12 Right bottom left
		size, -size, size,  // 13 Right bottom right
		size, size, size,  // 14 Right top right
		size, size, -size,  // 15 Right top left

		-size, -size, size, // 16 Bottom top left
		size, -size, size,  // 17 Bottom top right
		size, -size, -size,  // 18 Bottom bottom right
		-size, -size, -size, // 19 Bottom bottom left

		-size, size, -size, // 20 Top bottom left
		size, size, -size,  // 21 Top bottom right
		size, size, size,  // 22 Top top right
		-size, size, size  // 23 Top top left
	};

	float uv[24 * 2] =
	{
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	// Color array (r, g, b, a)
	float* colors = (float*)malloc(sizeof(float) * 24 * 4);
	fill_n(colors, 24 * 4, 100.0f);

	Vertex vertices[24];
	FillVerticesData(24, uniqueVertex, colors, uv, vertices);

	glGenBuffers(1, (GLuint*) &(vertexBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 24, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLushort indicesArray[24 * 3] =
	{
		3, 1, 0,
		3, 2, 1,

		7, 5, 4,
		7, 6, 5,

		11, 9, 8,
		11, 10, 9,

		15, 13, 12,
		15, 14, 13,

		19, 17, 16,
		19, 18, 17,

		23, 21, 20,
		23, 22, 21
	};
	
	verticesNumber = sizeof(indicesArray)/sizeof(uint);

	glGenBuffers(1, (GLuint*) &(indicesBufferId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * verticesNumber, indicesArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float normal[24 * 3];
	float offset = 3;
	for(size_t i = 0; i < 36; i+=3)
	{
		int x1 = indicesArray[i] * offset;
		int x2 = indicesArray[i + 1] * offset;
		int x3 = indicesArray[i + 2] * offset;

		int y1 = (indicesArray[i] * offset) + 1;
		int y2 = (indicesArray[i + 1] * offset) + 1;
		int y3 = (indicesArray[i + 2] * offset) + 1;

		int z1 = (indicesArray[i] * offset) + 2;
		int z2 = (indicesArray[i + 1] * offset) + 2;
		int z3 = (indicesArray[i + 2] * offset) + 2;

		float edge1X = uniqueVertex[x2] - uniqueVertex[x1];
		float edge1Y = uniqueVertex[y2] - uniqueVertex[y1];
		float edge1Z = uniqueVertex[z2] - uniqueVertex[z1];

		float edge2X = uniqueVertex[x3] - uniqueVertex[x1];
		float edge2Y = uniqueVertex[y3] - uniqueVertex[y1];
		float edge2Z = uniqueVertex[z3] - uniqueVertex[z1];

		float3 vNormal = Cross(float3(edge1X, edge1Y, edge1Z), float3(edge2X, edge2Y, edge2Z)).Normalized();
		normal[i] = vNormal.x;
		normal[i + 1] = vNormal.y;
		normal[i + 2] = vNormal.z;
	}

	for(size_t i = 0; i < 36; i++)
	{
		LOG_DEBUG("%f",normal[i]);
	}

	glGenBuffers(1, (GLuint*) &(normalBufferId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, normalBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * 24 * 3, normal, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	RELEASE(colors);
}

void Mesh::SetUpSphere()
{
	drawMode = GL_QUADS;

	uint rings = 100;
	uint sectors = 100;
	uint radius = 1;

	float R = 1.0f / (float)(rings - 1);
	float S = 1.0f / (float)(sectors - 1);

	vector<GLfloat> vertices;
	vector<GLushort> indices;
	vector<GLfloat> texcoords;
	vector<GLfloat> normals;

	uint nVertices = rings * sectors;

	vertices.resize(nVertices * 3);
	texcoords.resize(nVertices * 2);
	normals.resize(nVertices * 3);

	vector<GLfloat>::iterator v = vertices.begin();
	vector<GLfloat>::iterator t = texcoords.begin();
	vector<GLfloat>::iterator n = normals.begin();
	for(unsigned int r = 0; r < rings; r++)
	{
		for(unsigned int s = 0; s < sectors; s++)
		{
			float y = (float)sin(-M_PI_2 + M_PI * r * R);
			float x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
			float z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

			*t++ = s * S;
			*t++ = r * R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
		}
	}

	// Color array (r, g, b, a)
	float* colors = (float*)malloc(sizeof(float) * nVertices * 4);
	fill_n(colors, nVertices * 4, 100.0f);

	// Cannot Vertex vVertices[nVertices];
	Vertex* vVertices = (Vertex*)malloc(sizeof(Vertex) * nVertices * 3);
	FillVerticesData(nVertices, vertices.data(), colors, texcoords.data(), vVertices);

	glGenBuffers(1, (GLuint*) &(vertexBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nVertices, vVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(normalBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * nVertices*3, ((void*)&normals[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indices.resize(rings * sectors * 4);
	verticesNumber = indices.size();

	vector<GLushort>::iterator i = indices.begin();

	for(unsigned int r = 0; r < rings - 1; r++)
	{
		for(unsigned int s = 0; s < sectors - 1; s++)
		{
			*i++ = (r + 1) * sectors + s;
			*i++ = (r + 1) * sectors + s + 1;
			*i++ = r * sectors + s + 1;
			*i++ = r * sectors + s;
		}
	}

	glGenBuffers(1, (GLuint*) &(indicesBufferId));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * verticesNumber, ((void*)&indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	RELEASE(colors);
	RELEASE(vVertices);
}

void Mesh::FillVerticesData(uint n, const float* positions, const float* colors, const float* texCoords, Vertex* vertices) const
{
	for(uint i = 0; i < n; ++i)
	{
		for(uint j = 0; j < 3; ++j)
		{
			vertices[i].position[j] = positions[i * 3 + j];
		}

		for(uint j = 0; j < 4; ++j)
		{
			vertices[i].color[j] = colors[i * 4 + j];
		}

		for(uint j = 0; j < 2; ++j)
		{
			vertices[i].uv[j] = texCoords[i * 2 + j];
		}
	}
}