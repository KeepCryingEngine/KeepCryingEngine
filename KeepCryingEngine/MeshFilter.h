#ifndef _MESHFILTER_H_
#define _MESHFILTER_H_

#include "Component.h"

class MeshEntity;

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

	MeshEntity* GetMesh() const;
	void SetMeshMode(MeshMode mode);

private:
	void SetMesh(MeshEntity* mesh);

private:
	MeshEntity * mesh = nullptr;
	MeshMode mode = MeshMode::CUBE;
};

#endif // !_MESHFILTER_H_




