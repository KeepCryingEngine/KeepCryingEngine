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
		}
		UpdateGameObjectAABB();
	}
}

void MeshFilter::UpdateGameObjectAABB()
{

	Transform* transform = (Transform*)gameObject->GetComponent(ComponentType::Transform);
	OBB obb = mesh->GetAABB().ToOBB();

	obb.Transform(transform->GetWorldRotation());
	obb.Translate(transform->GetWorldPosition());
	obb.Scale(transform->GetWorldPosition(), transform->GetWorldScale());

	gameObject->GetAABB().SetFrom(obb);
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

