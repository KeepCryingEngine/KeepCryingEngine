#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <MathGeoLib.h>

#include "Component.h"
#include "Globals.h"

struct camVertex
{
	float3 pos;
	float4 color;
};

class Camera : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Camera;

	Camera();
	virtual ~Camera();

	virtual void Start() override;
	// virtual void Start() override;
	virtual void Destroy() override;

	virtual void SetEnable(bool setEnable);

	void RealUpdate() override;

	void SetFOV(float radians);
	void SetAspectRatio(float aspect);
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);

	const Frustum& GetFrustum() const;

	float GetFOV() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	float4x4 GetViewMatrix() const;
	float4x4 GetProyectionMatrix() const;
	uint GetFrustumBufferId() const;
	uint GetFrustumIndicesId() const;
	int GetNumberOfPoints()const;
	
	void SetUpFrustum(const float3& position = float3::zero, const Quat& rotation = Quat::identity, float nearPlaneDistance = 0.1f, float farPlaneDistance = 50.0f, float fov = 60.0f);

	void DrawUI() override;

	virtual std::vector<Component::Type> GetNeededComponents() const override;
	virtual std::vector<Component::Type> GetProhibitedComponents() const override;

	static bool Intersects(const Frustum& frustum, const AABB& aabb);

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	float ComputeHorizontalFov(float verticalFovRad, float aspect) const;
	void SetUpFrustumBuffer();

private:
	Frustum frustum;
	uint frustumBufferId = 0;
	uint frustumIndicesId = 0;
	int numberOfPoints;
};

#endif