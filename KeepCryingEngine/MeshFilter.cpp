#include "MeshFilter.h"

#include "MeshRenderer.h"
#include "GameObject.h"

MeshFilter::MeshFilter() : Component(ComponentType::MeshFilter)
{
}

MeshFilter::~MeshFilter()
{
}

void MeshFilter::DrawUI()
{
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
