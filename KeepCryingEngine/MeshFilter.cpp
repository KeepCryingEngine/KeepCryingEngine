#include "MeshFilter.h"

#include "Application.h"
#include "ModuleEntity.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "MeshEntity.h"
#include "Transform.h"

MeshFilter::MeshFilter() : Component(ComponentType::MeshFilter)
{
	SetMesh(App->entity->GetCube());
}

MeshFilter::~MeshFilter()
{
}

void MeshFilter::DrawUI()
{
	if (ImGui::CollapsingHeader("MeshFilter"))
	{
		int uiMode = (int)mode;

		if(ImGui::Combo("Mesh", &uiMode,"Cube\0Sphere"))
		{
			mode = (MeshMode)uiMode;
			SetMeshMode(mode);
		}
	}
}

std::vector<ComponentType> MeshFilter::GetProhibitedComponents() const
{
	return { ComponentType::MeshFilter };
}

void MeshFilter::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if (mesh)
	{
		MeshRenderer* meshRenderer = (MeshRenderer*)gameObject->GetComponent(ComponentType::MeshRenderer);
		if (meshRenderer)
		{
			meshRenderer->Render(*mesh);
			if (debugAABB)
			{
				DrawGameObjectAABB();
			}
		}
		UpdateGameObjectAABB();
	}
}

void MeshFilter::UpdateGameObjectAABB()
{
	Transform* transform = (Transform*)gameObject->GetComponent(ComponentType::Transform);
	OBB obb = mesh->GetAABB().ToOBB();
	obb.Transform(transform->GetModelMatrix());

	gameObject->GetAABB().SetFrom(obb);
}

void MeshFilter::DrawGameObjectAABB()
{
	float3 aabbcorners[8];
	gameObject->GetAABB().GetCornerPoints(aabbcorners);
	
	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	
	glBegin(GL_LINES);
	glColor4f(255.0f, 255.0f, 255.0f, 1.0f);
	
	for(int i = 0; i < 8; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(i != j)
			{
				glVertex3f(aabbcorners[i].x, aabbcorners[i].y, aabbcorners[i].z);
				glVertex3f(aabbcorners[j].x, aabbcorners[j].y, aabbcorners[j].z);
			}
		}
	}
	
	glColor4f(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
	glEnd();	
}

MeshEntity * MeshFilter::GetMesh() const
{
	return mesh;
}

void MeshFilter::SetMeshMode(MeshMode mode)
{
	this->mode = mode;
	switch(mode)
	{
		case MeshMode::CUBE:
		{
			SetMesh(App->entity->GetCube());
		}
			break;
		case MeshMode::SPHERE:
		{
			SetMesh(App->entity->GetSphere());
		}
			break;
	}
}

void MeshFilter::SetMesh(MeshEntity * mesh)
{
	this->mesh = mesh;
}

