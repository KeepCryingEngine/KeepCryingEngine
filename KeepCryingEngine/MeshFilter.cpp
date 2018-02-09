#include "MeshFilter.h"

#include "MeshRenderer.h"
#include "GameObject.h"
#include "MeshEntity.h"

MeshFilter::MeshFilter() : Component(ComponentType::MeshFilter)
{
	mesh = new MeshEntity();
}

MeshFilter::~MeshFilter()
{
}

void MeshFilter::DrawUI()
{
	if (ImGui::CollapsingHeader("MeshFilter"))
	{
		
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

void MeshFilter::SetMesh(MeshEntity * mesh)
{
	this->mesh = mesh;
}

