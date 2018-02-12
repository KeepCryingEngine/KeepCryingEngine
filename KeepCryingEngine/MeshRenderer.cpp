#include "MeshRenderer.h"

#include "Application.h"
#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshFilter.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleUI.h"
#include "Camera.h"

MeshRenderer::MeshRenderer() : Component(ComponentType::MeshRenderer)
{
	material = new Material();
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

void MeshRenderer::Render(Mesh& mesh)
{
	if(enabled)
	{
		if(material)
		{
			Transform* transform = gameObject->GetTransform();
			if(transform)
			{
				if(gameObject->IsStatic())
				{
					if(gameObject->GetVisible())
					{
						App->renderer->AddToDrawBuffer(mesh, *material, *gameObject, *transform);
					}
				}
				else
				{
					if(App->ui->GetFrustumCulling())
					{
						if(App->camera->GetEnabledCamera() != nullptr && App->camera->GetEnabledCamera()->Intersects(App->camera->GetEnabledCamera()->GetFrustum(), gameObject->GetAABB()))
						{
							App->renderer->AddToDrawBuffer(mesh, *material, *gameObject, *transform);
						}
					}
					else
					{
						App->renderer->AddToDrawBuffer(mesh, *material, *gameObject, *transform);
					}
				}
			}
		}
	}
}

void MeshRenderer::SetMaterial(Material & material)
{
	this->material = &material;
}

Material * MeshRenderer::GetMaterial() const
{
	return material;
}