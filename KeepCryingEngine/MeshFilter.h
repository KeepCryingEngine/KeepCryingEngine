#ifndef _MESHFILTER_H_
#define _MESHFILTER_H_

#include "Component.h"

class MeshEntity;

class MeshFilter : public Component
{
public:
	MeshFilter();
	~MeshFilter();

	void DrawUI() override;
	std::vector<ComponentType> GetProhibitedComponents() const override;

	void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;

	MeshEntity* GetMesh() const;

private:
	MeshEntity * mesh;
};

#endif // !_MESHFILTER_H_




