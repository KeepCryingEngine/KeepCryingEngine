#ifndef _MESHFILTER_H_
#define _MESHFILTER_H_

#include "Component.h"

class Mesh;

enum class MeshMode
{
	CUBE,
	SPHERE,
};

class MeshFilter : public Component
{
public:
	MeshFilter();
	~MeshFilter();

	void DrawUI() override;
	std::vector<ComponentType> GetProhibitedComponents() const override;

	void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;

	Mesh* GetMesh() const;
	void SetMesh(Mesh* mesh);
	void SetMeshMode(MeshMode mode);

private:
	void UpdateGameObjectAABB();
	void DrawGameObjectAABB();

private:
	Mesh * mesh = nullptr;

	MeshMode mode = MeshMode::CUBE;
	bool debugAABB = true;
};

#endif // !_MESHFILTER_H_




