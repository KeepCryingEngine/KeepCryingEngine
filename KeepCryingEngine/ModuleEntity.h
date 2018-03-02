#ifndef _MODULEENTITY_H_
#define _MODULEENTITY_H_

#include <vector>
#include <experimental/filesystem>

#include "Module.h"
#include "Mesh.h"

struct aiNode;
struct aiScene;
class GameObject;
class Material;
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
	void LoadMesh(const std::experimental::filesystem::path& path);
private:
	
	void SetUpCube();
	void SetUpSphere();
	void GetCubeMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum& drawMode) const;
	void GetSphereMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum& drawMode) const;

	void FillVerticesData(std::vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const;

	void LoadMeshRecursive(const aiScene* scene,aiNode * currentChild,GameObject* father,const std::vector<Material*>& tempMaterials, const std::vector<Mesh*>& tempMeshes);


private:
	Mesh* cube;
	Mesh* sphere;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
};

#endif // !_MODULEENTITY_H_
