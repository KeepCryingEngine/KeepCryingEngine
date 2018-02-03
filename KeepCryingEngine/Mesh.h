#ifndef _MESH_H_
#define _MESH_H_

#include "Component.h"

#include "Globals.h"

struct Vertex
{
	float position[3];
	float color[4];
	float uv[2];
};

class Mesh : public Component
{
public:
	Mesh();
	~Mesh();

	void Update(float deltaTimeS, float realDeltaTimeS)override;
	void DrawUI()override;

private:
	void SetUpCube();
	void SetUpSphere();
	void FillVerticesData(uint n, const float* positions, const float* colors, const float* texCoords, Vertex* vertices) const;

private:
	uint vertexBufferId;
	uint indicesBufferId;
	uint verticesNumber;
};

#endif // !_MESH_H_

