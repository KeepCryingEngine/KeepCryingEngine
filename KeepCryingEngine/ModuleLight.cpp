#include "ModuleLight.h"

#include <Quat.h>
#include <MathFunc.h>
#include <vector>
#include <OBB.h>

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleEditorUI.h"
#include "ModuleScene.h"

const float3 LIGHT_POSITION_COLOR = float3 { 255, 255, 0 };
const float3 LIGHT_DIRECTION_COLOR = float3 { 255, 255, 0 };

using namespace std;

ModuleLight::ModuleLight()
{ }

ModuleLight::~ModuleLight()
{ }

bool ModuleLight::Start()
{
	frustum.type = OrthographicFrustum;
	frustum.nearPlaneDistance = 0.1f;
	ComputeFrustum();
	return true;
}

update_status ModuleLight::Update()
{
	if(App->uiEditor->GetDebugMode())
	{
		DrawLight();
	}

	ComputeFrustum();

	return update_status::UPDATE_CONTINUE;
}


void ModuleLight::ComputeFrustum()
{
	frustum.front = currentDirection.Normalized();
	frustum.up = Quat::RotateFromTo(float3::unitZ, frustum.front) * float3::unitY;
	OBB sceneObb = App->scene->ComputeOBB();

	float3 farPoint = sceneObb.ExtremePoint(frustum.front);
	float3 nearPoint = sceneObb.ExtremePoint(-frustum.front);
	float3 topPoint = sceneObb.ExtremePoint(frustum.up);
	float3 bottomPoint = sceneObb.ExtremePoint(-frustum.up);
	float3 rightPoint = sceneObb.ExtremePoint(frustum.WorldRight());
	float3 leftPoint = sceneObb.ExtremePoint(-frustum.WorldRight());

	frustum.farPlaneDistance = sceneObb.r.y *2;
	frustum.orthographicHeight = sceneObb.r.z*2;
	frustum.orthographicWidth = sceneObb.r.x*2;
	float3 tmp=sceneObb.CenterPoint();
	frustum.pos = sceneObb.CenterPoint()-frustum.front * frustum.farPlaneDistance / 2.0f;
	SetUpFrustumBuffer();
}

const float3& ModuleLight::GetPosition() const
{
	return frustum.pos;
}

const float3& ModuleLight::GetDirection() const
{
	return currentDirection;
}

void ModuleLight::SetDirection(const float3& direction)
{
	baseDirection = direction;
	currentDirection = direction;
	ComputeFrustum();
}

void ModuleLight::SetRotation(float amount)
{
	// amount 0 .. 1
	// amount 0 .. 360

	Quat rotation = Quat::RotateY(DegToRad(360.0f * amount));

	currentDirection = rotation * baseDirection;
	ComputeFrustum();
}

const Frustum & ModuleLight::GetFrustum() const
{
	return frustum;
}

void ModuleLight::DrawLight() const
{
	App->renderer->DrawSphere(frustum.pos, LIGHT_POSITION_COLOR, 5.0f);
	App->renderer->DrawCross(frustum.pos + 10.0f * frustum.front, LIGHT_DIRECTION_COLOR, 100.0f);
	App->renderer->DrawLightFrustum();
	App->renderer->DrawOBB(App->scene->ComputeOBB(), float3 { 255.0f, 0.0f, 255.0f });
}

float4x4 ModuleLight::GetViewMatrix() const
{
	return float4x4(frustum.ViewMatrix()).Transposed();
}

float4x4 ModuleLight::GetProyectionMatrix() const
{
	return frustum.ProjectionMatrix().Transposed();
}

float4x4 ModuleLight::GetRotationMatrix() const
{
	// return Quat::RotateFromTo(frustum.up, float3::unitY).ToFloat4x4();

	float3 direction = frustum.front;
	float3 up = frustum.up;

	float4x4 matrix;
	matrix.SetIdentity();
	
	float3 xaxis = Cross(up, direction);
	xaxis.Normalize();

	float3 yaxis = Cross(direction, xaxis);
	yaxis.Normalize();

	matrix.SetRow(0, float4(xaxis, 0.0f));
	matrix.SetRow(2, float4(yaxis, 0.0f));
	matrix.SetRow(1, float4(-direction, 0.0f));

	return matrix;
}

uint ModuleLight::GetFrustumIndicesId() const
{
	return frustumIndicesId;
}

int ModuleLight::GetNumberOfPoints() const
{
	return numberOfPoints;
}

void ModuleLight::SetBias(float value)
{
	bias = value;
}

float ModuleLight::GetBias() const
{
	return bias;
}

uint ModuleLight::GetFrustumBufferId() const
{
	return frustumBufferId;
}

void ModuleLight::SetUpFrustumBuffer()
{
	float3 tempPos = frustum.pos;
	frustum.pos = float3::zero;
	float3 points[8];
	frustum.GetCornerPoints(points);

	frustum.pos = tempPos;

	numberOfPoints = 24;
	vector<GLushort> orderedPoints = {
		0,2,
		2,6,
		6,4,
		4,0,
		1,3,
		3,7,
		7,5,
		5,1,
		0,1,
		2,3,
		6,7,
		4,5
	};

	vector<lightVertex> vertex;
	vertex.resize(8);

	for(int i = 0; i < 8; i++)
	{
		vertex[i].pos = points[i];
		vertex[i].color = float4(255.0f, 0.0f, 0.0f, 255.0f);
	}

	glDeleteBuffers(1, &frustumBufferId);
	glDeleteBuffers(1, &frustumIndicesId);

	glGenBuffers(1, (GLuint*) &(frustumBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, frustumBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertex) * 8, &vertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &frustumIndicesId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frustumIndicesId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numberOfPoints, &orderedPoints[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}