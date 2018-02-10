#ifndef _MODULEENTITY_H_
#define _MODULEENTITY_H_

#include "Module.h"
#include "MeshEntity.h"

class MeshEntity;

class ModuleEntity : public Module
{
public:
	ModuleEntity();
	~ModuleEntity();

	bool Init() override;
	bool CleanUp() override;

	MeshEntity* GetCube();
	MeshEntity* GetSphere();

private:
	void SetUpCube();
	void SetUpSphere();
	void GetCubeMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices) const;

	void FillVerticesData(std::vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const;


private:
	MeshEntity* cube;
	MeshEntity* sphere;
};

#endif // !_MODULEENTITY_H_
