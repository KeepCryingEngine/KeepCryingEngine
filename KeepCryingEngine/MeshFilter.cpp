#include "MeshFilter.h"

#include "Application.h"
#include "ModuleEntity.h"
#include "MeshRenderer.h"
#include "ModuleUI.h"
#include "GameObject.h"
#include "Mesh.h"
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
			if (App->ui->GetDebugMode())
			{
				DrawGameObjectAABB();
			}
		}
		UpdateGameObjectAABB();
	}
}

void MeshFilter::UpdateGameObjectAABB()
{
	Transform* transform = (Transform*)gameObject->GetComponent(ComponentType::Transform);
	OBB obb = mesh->GetAABB().ToOBB();
	obb.Transform(transform->GetModelMatrix());

	gameObject->GetAABB().SetFrom(obb);
}

void MeshFilter::DrawGameObjectAABB()
{
	float3 aabbcorners[8];
	gameObject->GetAABB().GetCornerPoints(aabbcorners);
	
	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	
	glBegin(GL_LINES);
	glColor4f(0.0f, 255.0f, 0.0f, 1.0f);
	
	glVertex3f(aabbcorners[0].x, aabbcorners[0].y, aabbcorners[0].z);
	glVertex3f(aabbcorners[1].x, aabbcorners[1].y, aabbcorners[1].z);
	glVertex3f(aabbcorners[1].x, aabbcorners[1].y, aabbcorners[1].z);
	glVertex3f(aabbcorners[3].x, aabbcorners[3].y, aabbcorners[3].z);
	glVertex3f(aabbcorners[3].x, aabbcorners[3].y, aabbcorners[3].z);
	glVertex3f(aabbcorners[2].x, aabbcorners[2].y, aabbcorners[2].z);
	glVertex3f(aabbcorners[2].x, aabbcorners[2].y, aabbcorners[2].z);
	glVertex3f(aabbcorners[0].x, aabbcorners[0].y, aabbcorners[0].z);

	glVertex3f(aabbcorners[4].x, aabbcorners[4].y, aabbcorners[4].z);
	glVertex3f(aabbcorners[5].x, aabbcorners[5].y, aabbcorners[5].z);
	glVertex3f(aabbcorners[5].x, aabbcorners[5].y, aabbcorners[5].z);
	glVertex3f(aabbcorners[7].x, aabbcorners[7].y, aabbcorners[7].z);
	glVertex3f(aabbcorners[7].x, aabbcorners[7].y, aabbcorners[7].z);
	glVertex3f(aabbcorners[6].x, aabbcorners[6].y, aabbcorners[6].z);
	glVertex3f(aabbcorners[6].x, aabbcorners[6].y, aabbcorners[6].z);
	glVertex3f(aabbcorners[4].x, aabbcorners[4].y, aabbcorners[4].z);

	glVertex3f(aabbcorners[7].x, aabbcorners[7].y, aabbcorners[7].z);
	glVertex3f(aabbcorners[3].x, aabbcorners[3].y, aabbcorners[3].z);
	glVertex3f(aabbcorners[5].x, aabbcorners[5].y, aabbcorners[5].z);
	glVertex3f(aabbcorners[1].x, aabbcorners[1].y, aabbcorners[1].z);
	glVertex3f(aabbcorners[4].x, aabbcorners[4].y, aabbcorners[4].z);
	glVertex3f(aabbcorners[0].x, aabbcorners[0].y, aabbcorners[0].z);
	glVertex3f(aabbcorners[6].x, aabbcorners[6].y, aabbcorners[6].z);
	glVertex3f(aabbcorners[2].x, aabbcorners[2].y, aabbcorners[2].z);
	
	glColor4f(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
	glEnd();	
}

Mesh * MeshFilter::GetMesh() const
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

void MeshFilter::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
}

