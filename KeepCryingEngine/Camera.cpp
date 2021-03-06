#include "Camera.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Transform.h"
#include "ModuleCamera.h"
#include "json_serializer.h"

using namespace std;

Camera::Camera() : Component(Camera::TYPE)
{
	SetUpCamera();
	App->camera->Subscribe(this);
}

Camera::~Camera()
{ }

void Camera::Start()
{
	App->camera->EnableCamera(this);
}

void Camera::Destroy()
{
	if(App->camera->GetEnabledCamera() == this)
	{
		App->camera->EnableCamera(nullptr);
	}
}

void Camera::RealUpdate()
{
	Transform* transform = gameObject->GetTransform();	

	frustum.pos = transform->GetWorldPosition();

	const Quat& worldRotation = transform->GetWorldRotation();
	frustum.front = worldRotation.Mul(float3::unitZ);
	frustum.up = worldRotation.Mul(float3::unitY);


	if(!ignoreFrustumRendering && App->state == TimeState::STOPED)
	{
		App->renderer->DrawFrustum(*this);
	}
}

void Camera::SetFOV(float radians)
{
	frustum.horizontalFov = ComputeHorizontalFov(radians, frustum.AspectRatio());
	frustum.verticalFov = radians;
	SetUpFrustumBuffer();
}

void Camera::SetAspectRatio(float aspect)
{
	frustum.horizontalFov = ComputeHorizontalFov(frustum.verticalFov, aspect);
	SetUpFrustumBuffer();
}

void Camera::SetNearPlane(float distance)
{
	frustum.nearPlaneDistance = distance;
	SetUpFrustumBuffer();
}

void Camera::SetFarPlane(float distance)
{
	frustum.farPlaneDistance = distance;
	SetUpFrustumBuffer();
}

const Frustum& Camera::GetFrustum() const
{
	return frustum;
}

float Camera::GetFOV() const
{
	return frustum.verticalFov;
}

float Camera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

float Camera::GetNearPlane() const
{
	return frustum.nearPlaneDistance;
}

float Camera::GetFarPlane() const
{
	return frustum.farPlaneDistance;
}

float4x4 Camera::GetViewMatrix() const
{
	return float4x4(frustum.ViewMatrix()).Transposed();
}

float4x4 Camera::GetProyectionMatrix() const
{
	return frustum.ProjectionMatrix().Transposed();
}

uint Camera::GetFrustumBufferId() const
{
	return frustumBufferId;
}

uint Camera::GetFrustumIndicesId() const
{
	return frustumIndicesId;
}

int Camera::GetNumberOfPoints() const
{
	return numberOfPoints;
}

void Camera::SetUpCamera(float nearPlaneDistance, float farPlaneDistance, float fov)
{
	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearPlaneDistance;
	frustum.farPlaneDistance = farPlaneDistance;
	frustum.verticalFov = DegToRad(fov);
	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.horizontalFov = ComputeHorizontalFov(frustum.verticalFov, (float)App->configuration.screenWidth/(float)App->configuration.screenHeight);
	SetUpFrustumBuffer();
}

void Camera::SetEnable(bool setEnable)
{
	if(setEnable)
	{
		enabled = wasEnabled;
	}
	else
	{
		enabled = setEnable;
	}

	App->camera->EnableCamera(enabled ? this : nullptr);
}

void Camera::SetIgnoreFrustumRendering(bool ignoreFrustumRendering)
{
	this->ignoreFrustumRendering = ignoreFrustumRendering;
}

void Camera::DrawUI()
{
	if(ImGui::CollapsingHeader("Camera"))
	{
		if(ImGui::Checkbox("Active", &enabled))
		{
			if(gameObject->IsEnabled())
			{
				wasEnabled = enabled;
				App->camera->EnableCamera(enabled ? this : nullptr);
			}
			else
			{
				enabled = false;
			}
		}
		
		ImGui::SameLine();
		
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
		float nearPlane = GetNearPlane();
		if(ImGui::DragFloat("Near plane", &nearPlane, 0.01f, 0.01f, 2.0f, "%.2f"))
		{
			SetNearPlane(nearPlane);
		}
		float farPlane = GetFarPlane();
		if(ImGui::DragFloat("Far plane", &farPlane, 2.0f, 10.0f, 1000.0f, "%.2f"))
		{
			SetFarPlane(farPlane);
		}
		float verticalFOV = GetFOV();
		if(ImGui::SliderFloat("Field of View", &verticalFOV, 0.1f, pi))
		{
			SetFOV(verticalFOV);
		}
		float aspectRatio = GetAspectRatio();
		if(ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.1f, 0.1f, 5.0f, "%.2f"))
		{
			SetAspectRatio(aspectRatio);
		}
	}
}

std::vector<Component::Type> Camera::GetNeededComponents() const
{
	return { Transform::TYPE };
}

std::vector<Component::Type> Camera::GetProhibitedComponents() const
{
	return { Camera::TYPE };
}

bool Camera::Intersects(const Frustum& frustum, const AABB& aabb)
{
	Plane planes[6];
	frustum.GetPlanes(planes);

	float3 corners[8];
	aabb.GetCornerPoints(corners);

	for(Plane plane : planes)
	{
		uint outsideCount = 0;

		for(float3 corner : corners)
		{
			if(plane.IsOnPositiveSide(corner))
			{
				++outsideCount;
			}
		}

		if(outsideCount == 8)
		{
			return false;
		}
	}

	return true;
}

void Camera::PreLoad(const nlohmann::json & json)
{
	Component::PreLoad(json);
	from_json(json["frustum"], frustum);
}

void Camera::Save(nlohmann::json& json) const
{
	Component::Save(json);
	nlohmann::json jsonFrustum;
	to_json(jsonFrustum, frustum);

	json["frustum"] = jsonFrustum;
}

float Camera::ComputeHorizontalFov(float verticalFovRad, float aspect) const
{
	return 2.0f * atan(tan(verticalFovRad / 2.0f) * aspect);
}

void Camera::SetUpFrustumBuffer()
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

	vector<camVertex> vertex;
	vertex.resize(8);

	for(int i=0;i < 8; i++)
	{
		vertex[i].pos = points[i];
		vertex[i].color = float4(255.0f, 0.0f, 0.0f, 255.0f);
	}

	glDeleteBuffers(1, &frustumBufferId);
	glDeleteBuffers(1, &frustumIndicesId);

	glGenBuffers(1, (GLuint*) &(frustumBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, frustumBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(camVertex) * 8, &vertex[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &frustumIndicesId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frustumIndicesId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numberOfPoints, &orderedPoints[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}