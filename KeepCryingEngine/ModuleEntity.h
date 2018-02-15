#ifndef _MODULEENTITY_H_
#define _MODULEENTITY_H_

#include "Module.h"
#include "Mesh.h"

class Mesh;

class ModuleEntity : public Module
{
public:
	ModuleEntity();
	~ModuleEntity();

	bool Init() override;
	bool CleanUp() override;

	Mesh* GetCube();
	Mesh* GetSphere();
	Mesh * LoadMesh(const char* path) const;
private:
	
	void SetUpCube();
	void SetUpSphere();
	void GetCubeMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum& drawMode) const;
	void GetSphereMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum& drawMode) const;

	void FillVerticesData(std::vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const;


private:
	Mesh* cube;
	Mesh* sphere;
};

#endif // !_MODULEENTITY_H_
