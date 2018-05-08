#include "ModuleEntity.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <assimp/scene.h>

#include "Globals.h"

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include "Application.h"
#include "ModuleEditorUI.h"
#include "ModuleScene.h"
#include "ModuleTexture.h"
#include "GameObject.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Transform.h"

using namespace std;

ModuleEntity::ModuleEntity()
{}

ModuleEntity::~ModuleEntity()
{}

bool ModuleEntity::Init()
{
	SetUpCube();
	SetUpSphere();
	SetUpPlane();
	return true;
}

bool ModuleEntity::CleanUp()
{
	RELEASE(cube);
	RELEASE(sphere);
	RELEASE(plane);
	return true;
}

Mesh * ModuleEntity::GetCube()
{
	return cube;
}

Mesh * ModuleEntity::GetSphere()
{
	return sphere;
}

Mesh * ModuleEntity::GetPlane()
{
	return plane;
}

void ModuleEntity::Load3DFile(const std::experimental::filesystem::path& path)
{
	const aiScene * scene = aiImportFile(path.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (scene != nullptr)
	{
		vector<Material*> createdMaterials;
		vector<Mesh*> createdMeshes;

		ExtractMaterialsFromScene(createdMaterials, scene, path);
		ExtractMeshesFromScene(createdMeshes, scene, path);

		materials.insert(materials.end(), createdMaterials.begin(), createdMaterials.end());
		meshes.insert(meshes.end(), createdMeshes.begin(), createdMeshes.end());

		GameObject* root = App->scene->Get(App->uiEditor->GetSelectedNode());
		LoadMeshRecursive(scene, scene->mRootNode, root, createdMaterials, createdMeshes);
	}

	//TODO we should release scene after it has been correctly imported
	//aiReleaseImport(scene); 
}

Mesh * ModuleEntity::ExtractNamedMeshFromScene(const aiScene * scene, const MeshIdentifier & meshIdentifier)
{
	for (size_t meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++)
	{
		aiMesh * aiMesh = scene->mMeshes[meshIndex];
		if (meshIdentifier.name == aiMesh->mName.C_Str()+meshIndex)
		{
			return ExtractMeshFromScene(scene, meshIndex, meshIdentifier.path);
		}
	}

	return nullptr;
}

void ModuleEntity::ExtractMaterialsFromScene(std::vector<Material *> &createdMaterials, const aiScene * scene, const std::experimental::filesystem::path & path) const
{
	createdMaterials.reserve(scene->mNumMaterials);

	std::experimental::filesystem::path baseTexturePath = path.parent_path();
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		aiString relativeTexturePath;
		scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &relativeTexturePath);
		std::experimental::filesystem::path texturePath(baseTexturePath);
		texturePath.append(relativeTexturePath.C_Str());
		
		Material* mat = new Material();
		mat->SetTextureByPath(texturePath);
		
		createdMaterials.push_back(mat);
	}
}

void ModuleEntity::ExtractMeshesFromScene(std::vector<Mesh *> &createdMeshes, const aiScene * scene, const std::experimental::filesystem::path& path) const
{
	createdMeshes.reserve(scene->mNumMeshes);
	for (size_t meshIndex = 0; meshIndex< scene->mNumMeshes; meshIndex++)
	{
		Mesh* mesh = ExtractMeshFromScene(scene, meshIndex, path);

		createdMeshes.push_back(mesh);
	}
}

Mesh * ModuleEntity::ExtractMeshFromScene(const aiScene* aiScene, size_t meshIndex, const std::experimental::filesystem::path & path) const
{
	aiMesh* aiMesh = aiScene->mMeshes[meshIndex];

	vector<Vertex> vertices;
	vector<GLushort> indices;
	vector<Bone> bones;

	ExtractVerticesAndIndicesFromScene(aiScene, aiMesh, vertices, indices);
	ExtractBonesFromMesh(aiScene, aiMesh, bones);

	string meshName = aiMesh->mName.C_Str() + meshIndex;
	MeshIdentifier meshIdentifier = { path, meshName };
	Mesh* mesh = new Mesh(meshIdentifier);
	mesh->SetMeshData(vertices, indices, bones, GL_TRIANGLES);

	return mesh;
}

void ModuleEntity::ExtractBonesFromMesh(const aiScene * scene, const aiMesh* mesh, std::vector<Bone> &bones) const
{
	bones.clear();
	bones.reserve(mesh->mNumBones);
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* aiBone = mesh->mBones[i];
		Bone bone;
		bone.name = aiBone->mName.C_Str();
		for (unsigned weightIndex = 0; weightIndex < aiBone->mNumWeights; weightIndex++)
		{
			Weigth weight;
			weight.vertex = aiBone->mWeights[weightIndex].mVertexId;
			weight.weight = aiBone->mWeights[weightIndex].mWeight;
			bone.weights.push_back(weight);
		}
		bone.bind = aiBone->mOffsetMatrix;
		bones.push_back(bone);
	}
}

void ModuleEntity::ExtractVerticesAndIndicesFromScene(const aiScene * scene, const aiMesh* mesh, std::vector<Vertex> &vertices, std::vector<GLushort> &indices) const
{	
	for (unsigned int k = 0; k < mesh->mNumVertices; k++)
	{
		Vertex vertex;
		vertex.position = float3(mesh->mVertices[k].x, mesh->mVertices[k].y, mesh->mVertices[k].z);
		if(mesh->HasNormals())
		{
			vertex.normal = float3(mesh->mNormals[k].x, mesh->mNormals[k].y, mesh->mNormals[k].z);
		}
		vertex.color = float4(100, 100, 100, 255);
		vertex.uv = float2(mesh->mTextureCoords[0][k].x, mesh->mTextureCoords[0][k].y);
		vertices.push_back(vertex);
	}
	for (unsigned int k = 0; k < mesh->mNumFaces; k++)
	{
		for (unsigned int m = 0; m < mesh->mFaces[k].mNumIndices; m++)
		{
			indices.push_back(mesh->mFaces[k].mIndices[m]);
		}
	}
}

void ModuleEntity::SetUpCube()
{
	vector<Vertex> vertices;
	vector<GLushort> indices;
	GLenum drawMode;
	GetCubeMeshData(vertices, indices, drawMode);

	MeshIdentifier meshIdentifier = { "ENGINE_DEFAULTS", "CUBE" };
	cube = new Mesh(meshIdentifier);
	cube->SetMeshData(vertices,indices, vector<Bone>(), drawMode);
	Register(cube);
}

void ModuleEntity::SetUpSphere()
{
	vector<Vertex> vertices;
	vector<GLushort> indices;
	GLenum drawMode;
	GetSphereMeshData(vertices, indices, drawMode);

	MeshIdentifier meshIdentifier = { "ENGINE_DEFAULTS", "SPHERE" };
	sphere = new Mesh(meshIdentifier);
	sphere->SetMeshData(vertices, indices, vector<Bone>(), drawMode);
	Register(sphere);
}

void ModuleEntity::SetUpPlane()
{
	vector<Vertex> vertices;
	vector<GLushort> indices;
	GLenum drawMode;
	GetPlaneMeshData(vertices, indices, drawMode);

	MeshIdentifier meshIdentifier = { "ENGINE_DEFAULTS", "PLANE" };
	plane = new Mesh(meshIdentifier);
	plane->SetMeshData(vertices, indices, vector<Bone>(), drawMode);
	Register(plane);
}

void ModuleEntity::GetCubeMeshData(vector<Vertex>& vertices, vector<GLushort>& indices, GLenum& drawMode) const
{
	assert(vertices.size() == 0);
	assert(indices.size() == 0);

	drawMode = GL_TRIANGLES;

	//Vertices
	{
		const float size = 1.0f;
		const size_t nCubeVertices = 24;
		vertices.resize(nCubeVertices);

		float3 positions[nCubeVertices] = {
		{ -size, -size, -size }, // 0 Front bottom left
		{ size, -size, -size }, // 1 Front bottom right
		{ size, size, -size }, // 2 Front top right
		{ -size, size, -size }, // 3 Front top left

		{ -size, -size, size }, // 4 Left bottom left
		{ -size, -size, -size }, // 5 Left bottom right
		{ -size, size, -size }, // 6 Left top right
		{ -size, size, size }, // 7 Left top left

		{ size, -size, size }, // 10 Back bottom right
		{ -size, -size, size }, // 11 Back bottom left
		{ -size, size, size }, // 8 Back top left
		{ size, size, size }, // 9 Back top right

		{ size, -size, -size }, // 12 Right bottom left
		{ size, -size, size }, // 13 Right bottom right
		{ size, size, size }, // 14 Right top right
		{ size, size, -size }, // 15 Right top left

		{ -size, -size, size }, // 16 Bottom top left
		{ size, -size, size }, // 17 Bottom top right
		{ size, -size, -size }, // 18 Bottom bottom right
		{ -size, -size, -size }, // 19 Bottom bottom left

		{ -size, size, -size }, // 20 Top bottom left
		{ size, size, -size }, // 21 Top bottom right
		{ size, size, size }, // 22 Top top right
		{ -size, size, size } // 23 Top top left
		};

		float4 colors[nCubeVertices];
		for (size_t i = 0; i < nCubeVertices; i++)
		{
			colors[i] = float4(100, 100, 100, 100);
		}

		float2 uv[nCubeVertices] = {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },

			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
		};

		float3 normals[nCubeVertices]; // TODO FILL THE NORMALS WITH THE NORMAL VALUE
		/*float offset = 3;
		for(size_t i = 0; i < 36; i+=3)
		{
		int x1 = indicesArray[i] * offset;
		int x2 = indicesArray[i + 1] * offset;
		int x3 = indicesArray[i + 2] * offset;

		int y1 = (indicesArray[i] * offset) + 1;
		int y2 = (indicesArray[i + 1] * offset) + 1;
		int y3 = (indicesArray[i + 2] * offset) + 1;

		int z1 = (indicesArray[i] * offset) + 2;
		int z2 = (indicesArray[i + 1] * offset) + 2;
		int z3 = (indicesArray[i + 2] * offset) + 2;

		float edge1X = uniqueVertex[x2] - uniqueVertex[x1];
		float edge1Y = uniqueVertex[y2] - uniqueVertex[y1];
		float edge1Z = uniqueVertex[z2] - uniqueVertex[z1];

		float edge2X = uniqueVertex[x3] - uniqueVertex[x1];
		float edge2Y = uniqueVertex[y3] - uniqueVertex[y1];
		float edge2Z = uniqueVertex[z3] - uniqueVertex[z1];

		float3 vNormal = Cross(float3(edge1X, edge1Y, edge1Z), float3(edge2X, edge2Y, edge2Z)).Normalized();
		normal[i] = vNormal.x;
		normal[i + 1] = vNormal.y;
		normal[i + 2] = vNormal.z;
		}*/

		FillVerticesData(vertices, positions, normals, colors, uv);
	}

	//Indices
	{
		const size_t nCubeIndices = 36;
		indices.reserve(nCubeIndices);
		indices = {
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
	}
}
	
void ModuleEntity::GetSphereMeshData(vector<Vertex>& vertices, vector<GLushort>& indices, GLenum& drawMode) const
{
	assert(vertices.size() == 0);
	assert(indices.size() == 0);

	drawMode = GL_QUADS;

	const uint rings = 100;
	const uint sectors = 100;

	//Vertices
	{
		const size_t nSphereVertices = rings * sectors;
		vertices.resize(nSphereVertices);

		uint radius = 1;

		float R = 1.0f / (float)(rings - 1);
		float S = 1.0f / (float)(sectors - 1);

		vector<float3> positions;
		vector<float2> texcoords;
		vector<float3> normals;


		positions.resize(nSphereVertices * 3);
		texcoords.resize(nSphereVertices * 2);
		normals.resize(nSphereVertices * 3);

		vector<float3>::iterator v = positions.begin();
		vector<float2>::iterator t = texcoords.begin();
		vector<float3>::iterator n = normals.begin();
		for (unsigned int r = 0; r < rings; r++)
		{
			for (unsigned int s = 0; s < sectors; s++)
			{
				float y = (float)sin(-M_PI_2 + M_PI * r * R);
				float x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
				float z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

				*t++ = float2(s * S, r * R);

				*v++ = float3(x * radius, y * radius, z * radius);

				*n++ = float3(x, y, z);
			}
		}

		float4 colors[nSphereVertices];
		for (size_t i = 0; i < nSphereVertices; i++)
		{
			colors[i] = float4(100, 100, 100, 100);
		}

		FillVerticesData(vertices, &positions[0], &normals[0], colors, &texcoords[0]);
	}

	//Indices
	{
		const size_t sphereIndicesNumber = rings * sectors * 4;
		indices.resize(sphereIndicesNumber);

		vector<GLushort>::iterator i = indices.begin();

		for (unsigned int r = 0; r < rings - 1; r++)
		{
			for (unsigned int s = 0; s < sectors - 1; s++)
			{
				*i++ = (r + 1) * sectors + s;
				*i++ = (r + 1) * sectors + s + 1;
				*i++ = r * sectors + s + 1;
				*i++ = r * sectors + s;
			}
		}
	}
}

void ModuleEntity::GetPlaneMeshData(std::vector<Vertex>& vertices, std::vector<GLushort>& indices, GLenum & drawMode) const
{
	assert(vertices.size() == 0);
	assert(indices.size() == 0);

	drawMode = GL_TRIANGLES;

	//Vertices
	{
		const float size = 1.0f;
		const size_t nPlaneVertices = 4;
		vertices.resize(nPlaneVertices);

		float3 positions[nPlaneVertices] = {
		{ -size, -size, -size }, // 0 Front bottom left
		{ size, -size, -size }, // 1 Front bottom right
		{ size, size, -size }, // 2 Front top right
		{ -size, size, -size }, // 3 Front top left
		};

		float4 colors[nPlaneVertices];
		for(size_t i = 0; i < nPlaneVertices; i++)
		{
			colors[i] = float4(100, 100, 100, 100);
		}

		float2 uv[nPlaneVertices] = {
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f },
		};

		float3 normals[nPlaneVertices] = {
		{ 0.0f,0.0f,1.0f },
		{ 0.0f,0.0f,1.0f },
		{ 0.0f,0.0f,1.0f },
		{ 0.0f,0.0f,1.0f }
		};

		FillVerticesData(vertices, positions, normals, colors, uv);
	}

	//Indices
	{
		const size_t nPlaneIndices = 6;
		indices.reserve(nPlaneIndices);
		indices = {
			3, 1, 0,
			3, 2, 1,
		};
	}
}

void ModuleEntity::FillVerticesData(vector<Vertex>& vertices, const float3 * positions, const float3* normals, const float4 * colors, const float2 * uvs) const
{
	assert(vertices.size() > 0);
	assert(positions);
	assert(normals);
	assert(colors);
	assert(uvs);

	for (GLuint i = 0; i < vertices.size(); ++i)
	{
		vertices[i].position = positions[i];
		vertices[i].normal = normals[i];
		vertices[i].color = colors[i];
		vertices[i].uv = uvs[i];
	}
}

void ModuleEntity::LoadMeshRecursive(const aiScene* scene,aiNode * currentNode, GameObject* father ,const vector<Material*>& materials, const vector<Mesh*>& meshes)
{
	GameObject* nodeGameObject = CreateGameObjectForNode(scene, currentNode, father, materials, meshes);
	for(unsigned int childIndex = 0; childIndex < currentNode->mNumChildren; childIndex++)
	{
		aiNode* child = currentNode->mChildren[childIndex];
		LoadMeshRecursive(scene, child, nodeGameObject, materials, meshes);
	}
}

GameObject* ModuleEntity::CreateGameObjectForNode(const aiScene* scene, aiNode * currentNode, GameObject* father, const vector<Material*>& materials, const vector<Mesh*>& meshes)
{
	GameObject * gameObject = App->scene->AddEmpty(*father, currentNode->mName.C_Str());

	aiVector3D rotation, scale, position;
	currentNode->mTransformation.Decompose(scale, rotation, position);

	Quat rotationQuat = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	gameObject->GetTransform()->SetLocalPosition(float3(position.x, position.y, position.z));
	gameObject->GetTransform()->SetLocalScale(float3(scale.x, scale.y, scale.z));
	gameObject->GetTransform()->SetLocalRotation(rotationQuat);

	for (unsigned int j = 0; j < currentNode->mNumMeshes; j++)
	{
		unsigned int meshIndex = currentNode->mMeshes[j];
		aiMesh * aiMesh = scene->mMeshes[meshIndex];
		Mesh* mesh = meshes[meshIndex];

		GameObject* currentGameobjectMesh = App->scene->AddEmpty(*gameObject, aiMesh->mName.C_Str());

		Material * material = materials[aiMesh->mMaterialIndex];
		currentGameobjectMesh->AddComponent<MeshRenderer>();
		currentGameobjectMesh->GetComponent<MeshRenderer>()->SetMaterial(*new Material(*material));

		currentGameobjectMesh->GetComponent<MeshFilter>()->SetMesh(mesh);
	}

	return gameObject;
}

Mesh * ModuleEntity::Load(const MeshIdentifier & identifier)
{
	Mesh * mesh = nullptr;
	const aiScene * scene = aiImportFile(identifier.path.string().c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (scene != nullptr)
	{
		mesh = ExtractNamedMeshFromScene(scene, identifier);
	}
	
	return mesh;
}

void ModuleEntity::Unload(Mesh * asset)
{
	delete asset;
}