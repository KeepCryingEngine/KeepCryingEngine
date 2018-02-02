#include "Mesh.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <GL/glew.h>
#include <vector>

#include "Application.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include "Material.h"
#include <typeinfo>

using namespace std;

Mesh::Mesh():Component(ComponentType::Mesh)
{
}

Mesh::~Mesh()
{
}

void Mesh::Update(float deltaTimeS, float realDeltaTimeS)
{
	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();
	uint progId = gameObject->GetComponent<Material>().GetProgramId;
	uint textId = gameObject->GetComponent<Material>().GetTextureId;

	glUseProgram(progId);
	if(textId != 0)
	{
		GLint texture = glGetUniformLocation(progId, "ourTexture");
		glUniform1i(texture, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textId);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	GLint modelView = glGetUniformLocation(progId, "model_view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->GetProyectionMatrix().ptr());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glDrawElements(GL_QUADS, verticesNumber, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glUseProgram(0);

	glPopMatrix();
}

void Mesh::DrawUI()
{
	ImGui::Begin("Material");
	ImGui::Checkbox("Active", &enabled);
	if(ImGui::Button("Delete Component"))
	{
		gameObject.RemoveComponent(this);
	}
	ImGui::End();
}

void Mesh::SetUpIndicesArray()
{
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

	float3 pos = gameObject->GetComponent<Transform>().position;
	for(size_t i = 0; i < 24 * 3; i += 3)
	{
		uniqueVertex[i] += pos.x;
		uniqueVertex[i + 1] += pos.y;
		uniqueVertex[i + 2] += pos.z;
	}

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
	uint indicesArray[36 * 3] =
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * verticesNumber, indicesArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	RELEASE(colors);
}

void Mesh::SetUpSphere()
{
	uint rings = 100;
	uint sectors = 100;
	uint radius = 1;

	float R = 1.0f / (float)(rings - 1);
	float S = 1.0f / (float)(sectors - 1);

	vector<GLfloat> vertices;
	vector<GLushort> indices;
	vector<GLfloat> texcoords;

	uint nVertices = rings * sectors;

	vertices.resize(nVertices * 3);
	texcoords.resize(nVertices * 2);

	vector<GLfloat>::iterator v = vertices.begin();
	vector<GLfloat>::iterator t = texcoords.begin();
	float3 pos = gameObject->GetComponent<Transform>().position;
	for(unsigned int r = 0; r < rings; r++)
	{
		for(unsigned int s = 0; s < sectors; s++)
		{
			float y = (float)sin(-M_PI_2 + M_PI * r * R);
			float x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
			float z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

			*t++ = s * S;
			*t++ = r * R;

			*v++ = x * radius + pos.x;
			*v++ = y * radius + pos.y;
			*v++ = z * radius + pos.z;
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

	indices.resize(rings * sectors * 4);
	verticesNumber = indices.size();

	vector<GLushort>::iterator i = indices.begin();

	for(unsigned int r = 0; r < rings - 1; r++)
	{
		for(unsigned int s = 0; s < sectors - 1; s++)
		{
			*i++ = r * sectors + s;
			*i++ = r * sectors + s + 1;
			*i++ = (r + 1) * sectors + s + 1;
			*i++ = (r + 1) * sectors + s;
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