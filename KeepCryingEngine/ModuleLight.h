#ifndef _MODULELIGHT_H_
#define _MODULELIGHT_H_

#include <float3.h>
#include <float4.h>
#include <float4x4.h>
#include <Frustum.h>

#include "Module.h"

struct lightVertex
{
	float3 pos;
	float4 color;
};

class ModuleLight : public Module
{
public:
	ModuleLight();
	virtual ~ModuleLight();
	
	bool Start()override;
	update_status Update() override;

	const float3& GetPosition() const;
	void SetPosition(const float3& position);

	const float3& GetDirection() const;
	void SetDirection(const float3& direction);

	void SetRotation(float amount);

	const Frustum& GetFrustum()const;
	float4x4 GetViewMatrix()const;
	float4x4 GetProyectionMatrix() const;

	float4x4 GetRotationMatrix() const;

	uint GetFrustumBufferId()const;
	uint GetFrustumIndicesId()const;
	int GetNumberOfPoints()const;

	void SetBias(float value);
	float GetBias()const;

private:
	void DrawLight() const;
	void ComputeFrustum();
	void SetUpFrustumBuffer();

private:
	const float3 INITIAL_POSITION = float3{ 0.0f, 50.0f, 0.0f };
	const float3 INITIAL_ROTATION = float3{ 0.0f, -1.0f, 0.0f };

	float3 basePosition = INITIAL_POSITION;
	float3 currentPosition = INITIAL_POSITION;

	float3 baseDirection = INITIAL_ROTATION;
	float3 currentDirection = INITIAL_ROTATION;

	Frustum frustum;
	uint frustumBufferId = 0;
	uint frustumIndicesId = 0;
	int numberOfPoints;
	float bias = 0.001f;
};

#endif