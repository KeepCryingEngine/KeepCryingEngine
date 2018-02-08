#include "MeshRenderer.h"

#include "MeshEntity.h"
#include "MaterialEntity.h"
#include "GameObject.h"
#include "MeshFilter.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
}


MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::DrawUI()
{
}

std::vector<ComponentType> MeshRenderer::GetProhibitedComponents() const
{
	return { ComponentType::MeshRenderer };
}

void MeshRenderer::Render(MeshEntity& mesh)
{
	if (material)
	{
		
	}
}

void MeshRenderer::SetMaterial(MaterialEntity & material)
{
	this->material = &material;
}

MaterialEntity * MeshRenderer::GetMaterial() const
{
	return material;
}
