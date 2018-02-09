#ifndef _MESHENTITY_H_
#define _MESHENTITY_H_

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

class MeshEntity
{
public:
	MeshEntity();
	virtual ~MeshEntity();

	const AABB& GetAABB() const;
	GLuint GetVertexBufferId() const;
	GLuint GetIndicesBufferId() const;
	GLsizei GetVerticesNumber() const;

private:
	void GenerateBuffers(Vertex* vertices, size_t verticesSize, GLushort* indices, size_t indicesSize);
	
	void SetUpCube();

	void FillVerticesData(Vertex* vertices, GLuint nVertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const;
	void CalculateAABBForMesh(float3 * verticesPositions, size_t nVertices);

private:
	AABB aabb;
	GLuint vertexBufferId = 0;
	GLuint indicesBufferId = 0;
	GLsizei nVertices = 0;
	GLsizei nIndices = 0;

};

#endif // !_MESHENTITY_H_



