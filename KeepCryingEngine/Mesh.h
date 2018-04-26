#ifndef _MESH_H_
#define _MESH_H_

#include <experimental/filesystem>
#include <string>

#include <GL/glew.h>
#include <AABB.h>
#include <MathGeoLib.h>
#include <assimp/anim.h>

#include <vector>

#include "Asset.h"

struct Weigth
{
	unsigned vertex = 0;
	float weight = 0.0f;
};

struct Bone
{
	std::string name;
	std::vector<Weigth> weights;
	aiMatrix4x4 bind;
};

struct Vertex
{
	float3 position;
	float3 normal;
	float4 color;
	float2 uv;
};

struct MeshIdentifier
{
	std::experimental::filesystem::path path;
	std::string name;

	bool operator<(const MeshIdentifier& other) const;
};

class Mesh : public Asset<MeshIdentifier>
{
public:
	Mesh(const MeshIdentifier& meshIdentifier);
	virtual ~Mesh();
	
	void SetMeshData(const std::vector<Vertex>& vertices, const std::vector<GLushort>& indices, const std::vector<Bone> bones, GLenum drawMode);

	const AABB& GetAABB() const;
	GLuint GetVertexBufferId() const;
	GLuint GetIndicesBufferId() const;
	GLuint GetBoneIndicesBufferId() const;
	GLuint GetBoneWeightsBufferId() const;
	GLsizei GetVerticesNumber() const;
	GLsizei GetIndicesNumber() const;
	GLenum GetDrawMode()const;
	const std::vector<Vertex>& GetVertices() const;
	const std::vector<GLushort>& GetIndices() const;
	const std::vector<Vertex>& GetOriginalVertices() const;

	void UpdateVertices(const std::vector<Vertex>& vertices);

	const std::vector<Bone>& GetBones() const;

	void SetDynamicDraw(bool dynamicDraw);

private:
	void GenerateBuffers(const std::vector<Vertex>& vertices, const std::vector<GLushort>& indices);
	void CalculateAABBForMesh(const std::vector<Vertex> &vertices);

private:
	AABB aabb;
	GLuint vertexBufferId = 0;
	GLuint indicesBufferId = 0;
	GLuint boneIndicesBufferId = 0;
	GLuint boneWeightsBufferId = 0;
	GLsizei nVertices = 0;
	GLsizei nIndices = 0;
	GLenum drawMode = GL_TRIANGLES;

	std::vector<Bone> bones;
	std::vector<Vertex> vertices;
	std::vector<GLushort> indices;

	std::vector<Vertex> originalVertices;

	bool dynamicDraw = false;
};

#endif // !_MESHENTITY_H_



