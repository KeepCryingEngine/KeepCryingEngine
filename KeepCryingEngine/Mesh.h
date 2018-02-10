#ifndef _MESH_H_
#define _MESH_H_

#include <GL/glew.h>
#include <AABB.h>
#include <MathGeoLib.h>

struct Vertex
{
	float3 position;
	float3 normal;
	float4 color;
	float2 uv;
};

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	const AABB& GetAABB() const;
	GLuint GetVertexBufferId() const;
	GLuint GetIndicesBufferId() const;
	GLsizei GetVerticesNumber() const;
	GLsizei GetIndicesNumber() const;
	GLenum GetDrawMode()const;

	void SetMeshData(const std::vector<Vertex>& vertices, const std::vector<GLushort>& indices, GLenum drawMode);

private:
	void GenerateBuffers(const std::vector<Vertex> vertices, const std::vector<GLushort> indices);
	void CalculateAABBForMesh(const std::vector<Vertex> &vertices);

private:
	AABB aabb;
	GLuint vertexBufferId = 0;
	GLuint indicesBufferId = 0;
	GLsizei nVertices = 0;
	GLsizei nIndices = 0;
	GLenum drawMode = GL_TRIANGLES;

};

#endif // !_MESHENTITY_H_



