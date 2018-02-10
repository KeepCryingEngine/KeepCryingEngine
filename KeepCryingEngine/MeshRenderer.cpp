#include "MeshRenderer.h"

#include "Application.h"
#include "MeshEntity.h"
#include "MaterialEntity.h"
#include "GameObject.h"
#include "MeshFilter.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "Camera.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
	material = new MaterialEntity();
}


MeshRenderer::~MeshRenderer()
{
	delete material;
}

void MeshRenderer::DrawUI()
{
	if (ImGui::CollapsingHeader("MeshRenderer"))
	{
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

		if (material)
		{
			material->DrawUI();
		}
	}
}

std::vector<ComponentType> MeshRenderer::GetProhibitedComponents() const
{
	return { ComponentType::MeshRenderer };
}

void MeshRenderer::Render(MeshEntity& mesh)
{
	if (material)
	{
		Transform* transform = (Transform*)gameObject->GetComponent(ComponentType::Transform);
		if (transform)
		{
			Camera* enabledCamera = App->camera->GetEnabledCamera();
			if (enabledCamera != nullptr && enabledCamera->IsInsideFrustum(gameObject->GetAABB()))
			{
				App->renderer->AddToDrawBuffer(mesh, *material, *gameObject, *transform);
			}
		}
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