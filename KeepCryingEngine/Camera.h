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

	static bool Intersects(const Frustum& frustum, const AABB& aabb);

	Camera();
	virtual ~Camera();

	void Start() override;
	void RealUpdate() override;
	void Destroy() override;
	void SetEnable(bool setEnable) override;

	void SetIgnoreFrustumRendering(bool ignoreFrustumRendering);

	std::vector<Component::Type> GetNeededComponents() const override;
	std::vector<Component::Type> GetProhibitedComponents() const override;

	void DrawUI() override;

	void SetUpCamera(float nearPlaneDistance = 0.1f, float farPlaneDistance = 50.0f, float fov = 60.0f);
	const Frustum& GetFrustum() const;

	float4x4 GetViewMatrix() const;
	float4x4 GetProyectionMatrix() const;

	float GetFOV() const;
	void SetFOV(float radians);

	float GetAspectRatio() const;
	void SetAspectRatio(float aspect);

	float GetNearPlane() const;
	void SetNearPlane(float distance);

	float GetFarPlane() const;
	void SetFarPlane(float distance);

	//editor frustrum drawing
	uint GetFrustumBufferId() const;
	uint GetFrustumIndicesId() const;
	int GetNumberOfPoints()const;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	float ComputeHorizontalFov(float verticalFovRad, float aspect) const;
	void SetUpFrustumBuffer();

private:
	bool ignoreFrustumRendering = false;

	Frustum frustum;
	uint frustumBufferId = 0;
	uint frustumIndicesId = 0;
	int numberOfPoints;

};

#endif