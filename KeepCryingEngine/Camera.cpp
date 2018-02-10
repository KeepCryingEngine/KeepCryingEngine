#include "Camera.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleRender.h"
#include "Transform.h"
#include "ModuleCamera.h"

using namespace std;

Camera::Camera() : Component(ComponentType::Camera)
{
	SetUpFrustum(float3::zero, Quat::identity);
}

Camera::~Camera()
{ }

void Camera::Awake()
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

/* void Camera::Start()
{
	Transform* transform = (Transform*)gameObject->GetComponent(ComponentType::Transform);
	SetUpFrustum(transform->position, transform->rotation);
} */

void Camera::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	Transform* transform = (Transform*)gameObject->GetComponent(ComponentType::Transform);

	const float3& worldPosition = transform->GetWorldPosition();
	const Quat& worldRotation = transform->GetWorldRotation();
	
	// LOG_DEBUG("%s", position.ToString().c_str());

	frustum.pos = worldPosition;
	frustum.front = worldRotation.Mul(float3::unitZ);
	frustum.up = worldRotation.Mul(float3::unitY);

	// LOG_DEBUG("%s", frustum.pos.ToString().c_str());

	App->renderer->DrawFrustrum(*this);
}

void Camera::Translate(const float3& offset)
{
	frustum.Translate(offset);
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

void Camera::SetPosition(const float3& position)
{
	frustum.pos = position;
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

const float3& Camera::GetPosition() const
{
	return frustum.pos;
}

const float3& Camera::GetFrontVector() const
{
	return frustum.front;
}

void Camera::SetUpVector(const float3& upVector)
{
	frustum.up = upVector;
}

void Camera::SetFrontVector(const float3& frontVector)
{
	frustum.front = frontVector;
}

const float3& Camera::GetUpVector() const
{
	return frustum.up;
}

float3 Camera::GetSideVector() const
{
	return frustum.WorldRight();
	// return Cross(frustum.front, frustum.up);
}

void Camera::LookAt(const float3& point)
{
	float3 lookAtDirectionNormalized = (point - frustum.pos).Normalized();

	Quat lookAtRotation = Quat::LookAt(
		frustum.front,
		lookAtDirectionNormalized,
		frustum.up,
		float3::unitY
	);

	frustum.front = lookAtRotation.Mul(frustum.front);
	frustum.up = lookAtRotation.Mul(frustum.up);
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

int Camera::GetNumberOfPoints() const
{
	return numberOfPoints;
}

void Camera::SetUpFrustum(const float3& position, const Quat& rotation, float nearPlaneDistance, float farPlaneDistance, float fov)
{
	frustum.type = PerspectiveFrustum;
	frustum.pos = position; // float3(0, 1, -10);
	frustum.front = rotation.Mul(float3::unitZ); // float3::unitZ;
	frustum.up = rotation.Mul(float3::unitY); // float3::unitY;
	frustum.nearPlaneDistance = nearPlaneDistance;
	frustum.farPlaneDistance = farPlaneDistance;
	frustum.verticalFov = DegToRad(fov);
	frustum.horizontalFov = ComputeHorizontalFov(frustum.verticalFov, (float)App->configuration.screenWidth/(float)App->configuration.screenHeight);
	SetUpFrustumBuffer();
}

void Camera::DrawUI()
{
	if(ImGui::CollapsingHeader("Camera"))
	{
		if(ImGui::Checkbox("Active", &enabled))
		{
			App->camera->EnableCamera(enabled ? this : nullptr);
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
		if(ImGui::DragFloat("Aspect ratio", &aspectRatio, 0.1f, 0.1, 5, "%.2f"))
		{
			SetAspectRatio(aspectRatio);
		}
	}
}

std::vector<ComponentType> Camera::GetNeededComponents() const
{
	return { ComponentType::Transform };
}

std::vector<ComponentType> Camera::GetProhibitedComponents() const
{
	return { ComponentType::Camera };
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

float Camera::ComputeHorizontalFov(float verticalFovRad, float aspect) const
{
	return 2.0f * atan(tan(verticalFovRad / 2.0f) * aspect);
}

void Camera::SetUpFrustumBuffer()
{
	float3 tempPos = frustum.pos;
	frustum.pos = float3(0, 0, 0);
	float3 points[8];
	frustum.GetCornerPoints(points);
	assert(points);

	frustum.pos = tempPos;

	float3 orderedPoints[] = {
		points[0],points[2],
		points[2],points[6],
		points[6],points[4],
		points[4],points[0],
		points[1],points[3],
		points[3],points[7],
		points[7],points[5],
		points[5],points[1],
		points[0],points[1],
		points[2],points[3],
		points[6],points[7],
		points[4],points[5],
	};

	numberOfPoints=sizeof(orderedPoints)/sizeof(float3);

	glGenBuffers(1, (GLuint*) &(frustumBufferId));
	glBindBuffer(GL_ARRAY_BUFFER, frustumBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * numberOfPoints, orderedPoints, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
