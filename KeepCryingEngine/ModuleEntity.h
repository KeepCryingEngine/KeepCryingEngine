#ifndef _MODULEENTITY_H_
#define _MODULEENTITY_H_

#include <vector>
#include <map>
#include <experimental/filesystem>

#include "Module.h"
#include "Mesh.h"
#include "AssetManager.h"

struct aiNode;
struct aiScene;
struct aiMesh;
class GameObject;
class Material;

class ModuleEntity : public AssetManager<MeshIdentifier,Mesh>
{
public:
	ModuleEntity();
	~ModuleEntity();

	bool Init() override;
	bool CleanUp() override;

	Mesh* GetCube();
	Mesh* GetSphere();
	Mesh* GetPlane();
	void Load3DFile(const std::experimental::filesystem::path& path);

private:
	Mesh* ExtractNamedMeshFromScene(const aiScene * scene, const MeshIdentifier & meshIdentifier);

	void ExtractMaterialsFromScene(std::vector<Material *> &createdMaterials, const aiScene * scene, const std::experimental::filesystem::path & path) const;
	void ExtractMeshesFromScene(std::vector<Mesh *> &createdMeshes, const aiScene * scene, const std::experimental::filesystem::path& path) const;
	Mesh* ExtractMeshFromScene(const aiScene* aiScene, size_t meshIndex, const std::experimental::filesystem::path & meshIdentifier) const;
	void ExtractVertexDataFromScene(const aiScene * scene, const aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLushort> &indices, std::vector<float3>& tangents) const;
	void ExtractBonesFromMesh(const aiScene * scene, const aiMesh* mesh, std::vector<Bone> &bones) const;

	void SetUpCube();
	void SetUpSphere();
	void SetUpPlane();
	void GetCubeMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, std::vector<float3>& tangents, GLenum& drawMode) const;
	void GetSphereMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, std::vector<float3>& tangents, GLenum& drawMode) const;
	void GetPlaneMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, std::vector<float3>& tangents, GLenum& drawMode)const;

	void FillVerticesData(std::vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const;

	void LoadMeshRecursive(const aiScene* scene,aiNode * currentChild,GameObject* father,const std::vector<Material*>& tempMaterials, const std::vector<Mesh*>& tempMeshes);

	GameObject* CreateGameObjectForNode(const aiScene* scene, aiNode * currentNode, GameObject* father, const std::vector<Material*>& materials, const std::vector<Mesh*>& meshes);

protected:
	Mesh * Load(const MeshIdentifier & identifier) override;
	void Unload(Mesh * asset) override;

private:
	void CalculateTangents(const std::vector<Vertex>& vertices, const  std::vector<GLushort>& indices, std::vector<float3>& tangents,const GLenum& drawMode)const;
	void CalculateTangentsQuad(const std::vector<Vertex>& vertices, const  std::vector<GLushort>& indices, std::vector<float3>& tangents)const;
	void CalculateTangentsTri(const std::vector<Vertex>& vertices, const  std::vector<GLushort>& indices, std::vector<float3>& tangents)const;

private:
	Mesh* cube;
	Mesh* sphere;
	Mesh* plane;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
};

#endif // !_MODULEENTITY_H_
