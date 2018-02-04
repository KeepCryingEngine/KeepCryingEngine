#ifndef _MESH_H_
#define _MESH_H_

#include "Component.h"

#include "Globals.h"

#include <GL/glew.h>

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

	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;
	void DrawUI()override;

	void SetMeshMode(int mode);

private:
	void SetUpCube();
	void SetUpSphere();
	void FillVerticesData(uint n, const float* positions, const float* colors, const float* texCoords, Vertex* vertices) const;

private:
	GLenum drawMode;
	uint vertexBufferId;
	uint indicesBufferId;
	uint verticesNumber;

	bool changedMode = true;
	int meshMode = 0;
};

#endif // !_MESH_H_

