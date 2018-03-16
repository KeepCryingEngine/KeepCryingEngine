#include "GridRenderer.h"

#include "Material.h"
#include "Transform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleFX.h"
#include "Camera.h"

GridRenderer::GridRenderer() : Component(GridRenderer::TYPE)
{
	material = new Material();
}

GridRenderer::~GridRenderer()
{
	RELEASE(material);
}

void GridRenderer::Start()
{
	Build(rows,columns,size);
}

void GridRenderer::RealUpdate()
{
	UpdateBillboards();
	Render();
}

void GridRenderer::DrawUI()
{
	if(ImGui::CollapsingHeader("GridRenderer"))
	{
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

		if(ImGui::DragInt("Rows", &rows, 0.1f, 1, 1000000))
		{
			Build(rows, columns, size);
		}

		if(ImGui::DragInt("Columns", &columns, 0.1f, 1, 1000000))
		{
			Build(rows, columns, size);
		}

		if(ImGui::DragFloat2("Rows", size.ptr(), 0.1f, 1, 1000000))
		{
			Build(rows, columns, size);
		}

		if(material)
		{
			material->DrawUI();
		}
	}
}

std::vector<Component::Type> GridRenderer::GetProhibitedComponents() const
{
	return { GridRenderer::TYPE };
}

std::vector<Component::Type> GridRenderer::GetNeededComponents() const
{
	return { Transform::TYPE };
}

void GridRenderer::Render()
{
	BufferInfo temp = {numVertices,numIndices,vertexPosBufferId,vertexUvBufferId,indicesBufferId};
	App->fx->AddToDraw(*material,*gameObject->GetTransform(),temp);
}

void GridRenderer::SetMaterial(Material& material)
{
	this->material = &material;
}

Material* GridRenderer::GetMaterial() const
{
	return material;
}

void GridRenderer::Build(int rows, int columns, const float2& size)
{
	Clear();

	billboards.reserve(rows * columns);

	float2 bSize = { size.x / columns, size.y / rows };

	for(int r = 0; r < rows; ++r)
	{
		for(int c = 0; c < columns; ++c)
		{
			float3 bPosition = { (c * bSize.x + bSize.x / 2.0f)-size.x/2, bSize.y / 2.0f, (r * bSize.y + bSize.y / 2.0f) - size.y / 2 };

			Billboard* billboard = new Billboard();

			billboard->SetLocalPosition(bPosition);
			billboard->SetWorldPosition(bPosition + gameObject->GetTransform()->GetWorldPosition());
			billboard->SetSize(bSize);
			if(App->camera->GetEnabledCamera())
			{
				billboard->ComputeQuadInitial(*App->camera->GetEnabledCamera(), &vertexPos, &vertexUv, &indices);
			}
			else
			{
				billboard->ComputeQuadInitial(*App->camera->camera, &vertexPos, &vertexUv, &indices);
			}
			
			billboards.push_back(billboard);
		}
	}

	numVertices = vertexUv.size();
	numIndices = indices.size();

	//Generate Vertex Pos buffer
	const float3 * vertexPosPointer = &vertexPos[0];
	glGenBuffers(1, &vertexPosBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * numVertices, vertexPosPointer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Vertex Uv buffer
	const float2 * vertexUvPointer = &vertexUv[0];
	glGenBuffers(1, &vertexUvBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexUvBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * numVertices, vertexUvPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Indices buffer
	const GLushort* indicesPointer = &indices[0];
	glGenBuffers(1, &indicesBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, indicesPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GridRenderer::PreLoad(const nlohmann::json& json)
{

}

void GridRenderer::Save(nlohmann::json& json) const
{

}

void GridRenderer::UpdateBillboards()
{
	vertexPos.clear();

	for(Billboard* b : billboards)
	{
		b->SetWorldPosition(b->GetLocalPosition() + gameObject->GetTransform()->GetWorldPosition());
		if(App->camera->GetEnabledCamera())
		{
			b->ComputeQuad(*App->camera->GetEnabledCamera(), &vertexPos);
		}
		else
		{
			b->ComputeQuad(*App->camera->camera, &vertexPos);
		}
	}
	glDeleteBuffers(1, &vertexPosBufferId);

	//Generate Vertex Pos buffer
	const float3 * vertexPosPointer = &vertexPos[0];
	glGenBuffers(1, &vertexPosBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexPosBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * numVertices, vertexPosPointer, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GridRenderer::Clear()
{
	for(Billboard* billboard : billboards)
	{
		RELEASE(billboard);
	}

	vertexPos.clear();
	vertexUv.clear();
	indices.clear();

	billboards.clear();
}