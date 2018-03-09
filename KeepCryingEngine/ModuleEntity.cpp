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
	return true;
}

bool ModuleEntity::CleanUp()
{
	RELEASE(cube);
	RELEASE(sphere);
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

void ModuleEntity::LoadMesh(const std::experimental::filesystem::path& path)
{
	const aiScene * scene = aiImportFile(path.string().c_str(), aiProcess_Triangulate);

	if (scene != nullptr)
	{
		vector<Material*> createdMaterials;
		vector<Mesh*> createdMeshes;

		ExtractMaterialsFromScene(createdMaterials, scene, path);
		ExtractMeshesFromScene(createdMeshes, scene);

		materials.insert(materials.end(), createdMaterials.begin(), createdMaterials.end());
		meshes.insert(meshes.end(), createdMeshes.begin(), createdMeshes.end());

		GameObject* root = App->scene->Get(App->uiEditor->GetSelectedNode());
		LoadMeshRecursive(scene, scene->mRootNode, root, createdMaterials, createdMeshes);
	}
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
		mat->SetTexture(texturePath);

		createdMaterials.push_back(mat);
	}
}

void ModuleEntity::ExtractMeshesFromScene(std::vector<Mesh *> &createdMeshes, const aiScene * scene) const
{
	createdMeshes.reserve(scene->mNumMeshes);
	for (unsigned int meshIndex = 0; meshIndex< scene->mNumMeshes; meshIndex++)
	{
		aiMesh* aiMesh = scene->mMeshes[meshIndex];

		vector<Vertex> vertices;
		vector<GLushort> indices;

		ExtractVerticesAndIndicesFromScene(scene, *aiMesh, vertices, indices);

		Mesh* mesh = new Mesh();
		mesh->SetMeshData(vertices, indices, GL_TRIANGLES);

		createdMeshes.push_back(mesh);
	}
}

void ModuleEntity::ExtractVerticesAndIndicesFromScene(const aiScene * scene, aiMesh& mesh, std::vector<Vertex> &vertices, std::vector<GLushort> &indices) const
{
	bool addNormals = mesh.mNormals != nullptr;

	for (unsigned int k = 0; k < mesh.mNumVertices; k++)
	{
		Vertex vertex;
		vertex.position = float3(mesh.mVertices[k].x, mesh.mVertices[k].y, mesh.mVertices[k].z);
		if (addNormals)
		{
			vertex.normal = float3(mesh.mNormals[k].x, mesh.mNormals[k].y, mesh.mNormals[k].z);
		}
		vertex.color = float4(100, 100, 100, 255);
		vertex.uv = float2(mesh.mTextureCoords[0][k].x, mesh.mTextureCoords[0][k].y);
		vertices.push_back(vertex);
	}
	for (unsigned int k = 0; k < mesh.mNumFaces; k++)
	{
		for (unsigned int m = 0; m < mesh.mFaces[k].mNumIndices; m++)
		{
			indices.push_back(mesh.mFaces[k].mIndices[m]);
		}
	}
}

void ModuleEntity::SetUpCube()
{
	vector<Vertex> vertices;
	vector<GLushort> indices;
	GLenum drawMode;
	GetCubeMeshData(vertices, indices, drawMode);

	cube = new Mesh();
	cube->SetMeshData(vertices,indices, drawMode);
}

void ModuleEntity::SetUpSphere()
{
	vector<Vertex> vertices;
	vector<GLushort> indices;
	GLenum drawMode;
	GetSphereMeshData(vertices, indices, drawMode);

	sphere = new Mesh();
	sphere->SetMeshData(vertices, indices, drawMode);
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

void ModuleEntity::LoadMeshRecursive(const aiScene* scene,aiNode * currentChild, GameObject* father ,const vector<Material*>& tempMaterials, const vector<Mesh*>& tempMeshes)
{
	GameObject* currentGameobject = App->scene->AddEmpty(*father,currentChild->mName.C_Str());

	aiVector3D rotation, scale, position;
	currentChild->mTransformation.Decompose(scale, rotation, position);

	Quat rotationQuat = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	currentGameobject->GetTransform()->SetLocalPosition(float3(position.x, position.y, position.z));
	currentGameobject->GetTransform()->SetLocalScale(float3(scale.x, scale.y, scale.z));
	currentGameobject->GetTransform()->SetLocalRotation(rotationQuat);

	for(unsigned int i = 0; i < currentChild->mNumChildren; i++)
	{
		aiNode* child = currentChild->mChildren[i];
		if(child->mNumChildren > 0)
		{
			LoadMeshRecursive(scene, child, currentGameobject,tempMaterials,tempMeshes);
		}

		for(unsigned int j = 0; j < child->mNumMeshes; j++)
		{
			GameObject* currentGameobjectMesh = App->scene->AddEmpty(*currentGameobject, child->mName.C_Str());

			unsigned int  meshIndex = scene->mRootNode->mChildren[i]->mMeshes[j];

			currentGameobjectMesh->AddComponent<MeshRenderer>();
			currentGameobjectMesh->GetComponent<MeshFilter>()->SetMesh(tempMeshes[meshIndex]);
			int matIndex = scene->mMeshes[meshIndex]->mMaterialIndex;
			currentGameobjectMesh->GetComponent<MeshRenderer>()->SetMaterial(*new Material(*tempMaterials[matIndex]));
		}
	}
}
