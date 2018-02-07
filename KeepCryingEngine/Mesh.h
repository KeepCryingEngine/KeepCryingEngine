#ifndef _MESH_H_
#define _MESH_H_

#include "Globals.h"
#include "Component.h"

#include <GL/glew.h>

struct Vertex
{
	float position[3];
	float color[4];
	float uv[2];
};

enum class MeshMode
{
	CUBE,
	SPHERE
};

class Mesh : public Component
{
public:
	Mesh();
	virtual ~Mesh();

	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;
	void DrawUI() override;

	void SetMeshMode(MeshMode mode);

	virtual std::vector<ComponentType> GetNeededComponents() const override;
	virtual std::vector<ComponentType> GetProhibitedComponents() const override;

	GLenum GetDrawMode();
	uint GetVertexBufferId() const;
	uint GetIndicesBufferId() const;
	uint GetNormalBufferId() const;
	uint GetVerticesNumber() const;

private:
	void SetUpCube();
	void SetUpSphere();
	void FillVerticesData(uint n, const float* positions, const float* colors, const float* texCoords, Vertex* vertices) const;

private:
	GLenum drawMode;
	uint vertexBufferId;
	uint indicesBufferId;
	uint normalBufferId;
	uint verticesNumber;

	bool changedMode = true;
	MeshMode meshMode = MeshMode::CUBE;
};

#endif // !_MESH_H_

