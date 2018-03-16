#ifndef _MODULEENTITY_H_
#define _MODULEENTITY_H_

#include <vector>
#include <map>
#include <experimental/filesystem>

#include "Module.h"
#include "Mesh.h"

struct aiNode;
struct aiScene;
struct aiMesh;
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
	void Load3DFile(const std::experimental::filesystem::path& path);
	Mesh* LoadMesh(const std::experimental::filesystem::path& path, const std::string& name);

private:
	Mesh* ExtractNamedMeshFromScene(const aiScene * scene, const std::string& name, const std::experimental::filesystem::path& path);

	void ExtractMaterialsFromScene(std::vector<Material *> &createdMaterials, const aiScene * scene, const std::experimental::filesystem::path & path) const;
	void ExtractMeshesFromScene(std::vector<Mesh *> &createdMeshes, const aiScene * scene, const std::experimental::filesystem::path& path) const;
	Mesh* ExtractMeshFromScene(const aiScene* aiScene, size_t meshIndex, const std::experimental::filesystem::path& path) const;
	void ExtractVerticesAndIndicesFromScene(const aiScene * scene, const aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLushort> &indices) const;
	void ExtractBonesFromMesh(const aiScene * scene, const aiMesh* mesh, std::vector<Bone> &bones) const;

	void SetUpCube();
	void SetUpSphere();
	void GetCubeMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum& drawMode) const;
	void GetSphereMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum& drawMode) const;

	void FillVerticesData(std::vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const;

	void LoadMeshRecursive(const aiScene* scene,aiNode * currentChild,GameObject* father,const std::vector<Material*>& tempMaterials, const std::vector<Mesh*>& tempMeshes);

	GameObject* CreateGameObjectForNode(const aiScene* scene, aiNode * currentNode, GameObject* father, const std::vector<Material*>& materials, const std::vector<Mesh*>& meshes);
	void AddMeshToCache(Mesh* mesh);
	Mesh* GetFromMeshCache(const std::experimental::filesystem::path& path, const std::string& name);

private:
	Mesh* cube;
	Mesh* sphere;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::map<std::string, Mesh*> meshCache;
};

#endif // !_MODULEENTITY_H_
