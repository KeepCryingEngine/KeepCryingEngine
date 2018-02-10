#include "MeshFilter.h"

#include "Application.h"
#include "ModuleEntity.h"
#include "MeshRenderer.h"
#include "GameObject.h"
#include "MeshEntity.h"

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
		}
	}
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

