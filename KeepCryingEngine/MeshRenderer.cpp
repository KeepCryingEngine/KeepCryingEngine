#include "MeshRenderer.h"

#include "Brofiler.h"

#include "Application.h"
#include "Mesh.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshFilter.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleEditorUI.h"
#include "Camera.h"
#include "Texture.h"
#include "json_serializer.h"

MeshRenderer::MeshRenderer() : Component(MeshRenderer::TYPE)
{
	material = new Material();
}


MeshRenderer::~MeshRenderer()
{
	RELEASE(material);
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

std::vector<Component::Type> MeshRenderer::GetProhibitedComponents() const
{
	return { MeshRenderer::TYPE };
}

std::vector<Component::Type> MeshRenderer::GetNeededComponents() const
{
	return { MeshFilter::TYPE };
}

void MeshRenderer::Render(Mesh& mesh)
{
	BROFILER_CATEGORY("Render", Profiler::Color::Orchid)
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
					if(App->uiEditor->GetFrustumCulling())
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

void MeshRenderer::Load(const nlohmann::json& json)
{
	Material * material = new Material();
	material->SetShaderType(json["material"]["shaderType"]);
	material->SetTextureByPath(json["material"]["texture"]["path"].get<std::string>());
	material->GetTexture()->SetTextureConfiguration(json["material"]["texture"]["jsonConfiguration"]);
}

void MeshRenderer::Save(nlohmann::json& json) const
{
	json["type"] = type;

	nlohmann::json jsonMaterial;
	jsonMaterial["shaderType"] = material->GetShaderType();

	nlohmann::json jsonTexture;
	jsonTexture["path"] = material->GetPath().string();
	jsonTexture["jsonConfiguration"] = material->GetTexture()->GetTextureConfiguration();
	
	jsonMaterial["texture"] = jsonTexture;

	json["material"] = jsonMaterial;
}