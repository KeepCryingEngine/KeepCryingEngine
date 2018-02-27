#ifndef _MESHRENDERER_H_
#define _MESHRENDERER_H_

#include "Component.h"

class Material;
class Mesh;

class MeshRenderer : public Component
{
public:
	static const ComponentType TYPE = ComponentType::MeshRenderer;

	MeshRenderer();
	~MeshRenderer() override;

	void DrawUI() override;
	std::vector<ComponentType> GetProhibitedComponents() const override;
	std::vector<ComponentType> GetNeededComponents() const override;

	void Render(Mesh& mesh);

	void SetMaterial(Material& MaterialEntity);
	Material* GetMaterial() const;

private:
	Material * material = nullptr;
};

#endif // !_MESHRENDERER_H_



