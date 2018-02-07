#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <MathGeoLib.h>

#include "Component.h"
#include "Globals.h"

class Camera : public Component
{
public:
	Camera();
	virtual ~Camera();

	void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;

	void Translate(const float3& offset);

	void SetFOV(float radians);
	void SetAspectRatio();
	void SetNearPlane(float distance);
	void SetFarPlane(float distance);
	void SetPosition(const float3& position);

	float GetFOV() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;
	const float3& GetPosition() const;
	float4x4 GetViewMatrix() const;
	float4x4 GetProyectionMatrix() const;
	uint GetFrustumBufferId() const;
	float GetWidth()const;

	const float3& GetUpVector() const;
	float3 GetSideVector() const;
	const float3& GetFrontVector() const;

	void SetUpVector(const float3& upVector);
	void SetFrontVector(const float3& frontVector);

	void LookAt(const float3& point);
	
	void SetUpFrustum();

	void DrawUI() override;

	virtual std::vector<ComponentType> GetNeededComponents() const override;
	virtual std::vector<ComponentType> GetProhibitedComponents() const override;

private:
	float ComputeHorizontalFov(float radians, float width, float height) const;
	void SetUpFrustumBuffer();

private:
	Frustum frustum;
	uint frustumBufferId = 0;
};

#endif