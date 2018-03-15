#ifndef _BILLBOARD_H_
#define _BILLBOARD_H_

#include <float2.h>
#include <float3.h>

class Camera;
struct Vertex;

class Billboard
{
public:
	Billboard();
	virtual ~Billboard();

	void SetPosition(const float3& position);
	const float3& GetPosition() const;

	void SetSize(const float2& size);
	const float2& GetSize() const;

	void ComputeQuad(const Camera& camera, Vertex* outVertices) const;

private:

	float3 position;
	float2 size;
};

#endif