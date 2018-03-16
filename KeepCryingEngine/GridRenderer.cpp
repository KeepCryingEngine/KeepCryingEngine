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

		if(ImGui::DragFloat2("GridSize", size.ptr(), 0.1f, 1, 1000000))
		{
			Build(rows, columns, size);
		}

		if(ImGui::DragFloat2("BillboardSize", billboardSize.ptr(), 0.1f, 1, 1000000))
		{
			Build(rows, columns, size);
		}

		if(ImGui::DragFloat2("RadomPositionFactor", randomPosition.ptr(), 0.1f, 0, 5))
		{
			Build(rows, columns, size);
		}
		if(ImGui::DragFloat("RadomScaleFactor", &randomScale, 0.1f, 0, 5))
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

float RandomFloat(float min, float max)
{
	return  (max - min) * ((((float)rand()) / (float)RAND_MAX)) + min;
}

void GridRenderer::Build(int rows, int columns, const float2& size)
{
	Clear();

	float2 cellSize = { size.x / columns, size.y / rows };

	billboards.reserve(rows * columns);

	for(int r = 0; r < rows; ++r)
	{
		for(int c = 0; c < columns; ++c)
		{
			//RandomScale
			float2 currentBillboardSize = billboardSize + billboardSize*RandomFloat(-randomScale,randomScale);
			currentBillboardSize.x = max(currentBillboardSize.x,0);
			currentBillboardSize.y = max(currentBillboardSize.y, 0);
			//RandomPos
			float xPos =RandomFloat(-randomPosition.x,randomPosition.x) + (c * cellSize.x + cellSize.x / 2.0f) - size.x / 2;
			float yPos = currentBillboardSize.y / 2.0f;
			float zPos = RandomFloat(-randomPosition.y, randomPosition.y) + (r * cellSize.y + cellSize.y / 2.0f) - size.y / 2;
			float3 bPosition = { xPos,yPos ,zPos  };

			Billboard* billboard = new Billboard();

			billboard->SetLocalPosition(bPosition);
			billboard->SetWorldPosition(bPosition + gameObject->GetTransform()->GetWorldPosition());
			billboard->SetSize(currentBillboardSize);
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
	Component::PreLoad(json);
	rows = json["rows"];
	columns = json["columns"];
	from_json(json["size"],size);
	from_json(json["billboardSize"],billboardSize);
	from_json(json["randomPosition"],randomPosition);
	randomScale = json["randomScale"];

	material->SetShaderType(json["material"]["shaderType"]);
	material->SetTextureByPath(json["material"]["texture"]["path"].get<std::string>());
	material->GetTexture()->SetTextureConfiguration(json["material"]["texture"]["jsonConfiguration"]);
}

void GridRenderer::Save(nlohmann::json& json) const
{
	/*Material* material = nullptr;

	int rows = 1;
	int columns = 1;

	float2 size = float2::one;
	float2 billboardSize = float2::one;

	float2 randomPosition = float2::zero;
	float randomScale = 0.0f;*/

	Component::Save(json);
	json["rows"] = rows;
	json["columns"] = columns;
	nlohmann::json jsonSize;
	to_json(jsonSize,size);
	json["size"] = jsonSize;
	nlohmann::json jsonBillboardSize;
	to_json(jsonBillboardSize,billboardSize);
	json["billboardSize"] = jsonBillboardSize;
	nlohmann::json jsonRandomPosition;
	to_json(jsonRandomPosition,randomPosition);
	json["randomPosition"] = jsonRandomPosition;
	json["randomScale"] = randomScale;

	nlohmann::json jsonMaterial;
	jsonMaterial["shaderType"] = material->GetShaderType();

	nlohmann::json jsonTexture;
	jsonTexture["path"] = material->GetTexture()->Identifier().path.string();
	jsonTexture["jsonConfiguration"] = material->GetTexture()->GetTextureConfiguration();

	jsonMaterial["texture"] = jsonTexture;

	json["material"] = jsonMaterial;

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
