#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <float2.h>
#include <float3.h>
#include <GL/glew.h>

class Camera;
struct Vertex;

class Billboard
{
public:
	Billboard();
	virtual ~Billboard();

	void SetLocalPosition(const float3& position);
	void SetWorldPosition(const float3& position);
	const float3& GetWorldPosition() const;
	const float3& GetLocalPosition() const;

	void SetSize(const float2& size);
	const float2& GetSize() const;

	void ComputeQuad(const Camera& camera, std::vector<float3>* outverticesPos) const;
	void ComputeQuadInitial(const Camera& camera, std::vector<float3>* outverticesPos, std::vector<float2>* outverticesUv,std::vector<GLushort>* indices) const;

private:

	float3 localPosition;
	float3 worldPosition;
	float2 size;

	
};

#endif