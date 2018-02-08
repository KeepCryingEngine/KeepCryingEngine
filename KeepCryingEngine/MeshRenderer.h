#ifndef _MESHRENDERER_H_
#define _MESHRENDERER_H_

#include "Component.h"

class MaterialEntity;
class MeshEntity;

class MeshRenderer : public Component
{
public:
	MeshRenderer();
	~MeshRenderer() override;

	void DrawUI() override;
	std::vector<ComponentType> GetProhibitedComponents() const override;

	void Render(MeshEntity& mesh);

	void SetMaterial(MaterialEntity& MaterialEntity);
	MaterialEntity* GetMaterial() const;

private:
	MaterialEntity * material = nullptr;
};

#endif // !_MESHRENDERER_H_



