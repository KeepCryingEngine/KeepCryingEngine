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

void GridRenderer::RealUpdate()
{
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

void GridRenderer::Build(uint rows, uint columns, const float2& size)
{
	Clear();

	billboards.reserve(rows * columns);

	float2 bSize = { size.x / columns, size.y / rows };

	for(uint r = 0; r < rows; ++r)
	{
		for(uint c = 0; c < columns; ++c)
		{
			float3 bPosition = { c * bSize.x + bSize.x / 2.0f, bSize.y / 2.0f, r * bSize.y + bSize.y / 2.0f };

			Billboard* billboard = new Billboard();

			billboard->SetPosition(bPosition);
			billboard->SetSize(bSize);
			billboard->ComputeQuadInitial(*App->camera->GetEnabledCamera(),&vertexPos,&vertexUv,&indices);
		}
	}

	numVertices = vertexUv.size();
	numIndices = indices.size();

	//Generate Vertex buffer
	const Vertex * verticesPointer = &vertices[0];
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * nVertices, verticesPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Indices buffer
	const GLushort* indicesPointer = &indices[0];
	glGenBuffers(1, &indicesBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * nIndices, indicesPointer, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Vertex* vertices = new Vertex[4];
	// billboard->ComputeQuad(camera, vertices);
}

void GridRenderer::PreLoad(const nlohmann::json& json)
{

}

void GridRenderer::Save(nlohmann::json& json) const
{

}

void GridRenderer::Clear()
{
	for(Billboard* billboard : billboards)
	{
		RELEASE(billboard);
	}

	billboards.clear();
}