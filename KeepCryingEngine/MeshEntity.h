#ifndef _MESHENTITY_H_
#define _MESHENTITY_H_

#include <GL/glew.h>
#include <AABB.h>
#include <MathGeoLib.h>

struct Vertex
{
	float3 position;
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
	GLuint GetNormalBufferId() const;
	GLsizei GetVerticesNumber() const;

private:
	void SetUpCube();

	void FillVerticesData(Vertex* vertices, GLuint nVertices, const float3 * positions, const float4 * colors, const float2 * texCoords) const;
	void CalculateAABBForMesh(float * newVertices, size_t nVertices);

private:
	AABB aabb;
	GLuint vertexBufferId = 0;
	GLuint indicesBufferId = 0;
	GLuint normalBufferId = 0;
	GLsizei verticesNumber = 0;

};

#endif // !_MESHENTITY_H_



