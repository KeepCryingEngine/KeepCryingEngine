#ifndef _MESHRENDERER_H_
#define _MESHRENDERER_H_

#include "Component.h"

class Material;
class Mesh;

class MeshRenderer : public Component
{
public:
	static const Component::Type TYPE = Component::Type::MeshRenderer;

	MeshRenderer();
	~MeshRenderer() override;

	void DrawUI() override;
	std::vector<Component::Type> GetProhibitedComponents() const override;
	std::vector<Component::Type> GetNeededComponents() const override;

	void Render(Mesh& mesh);

	void SetMaterial(Material& MaterialEntity);
	Material* GetMaterial() const;

	virtual void Load(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	Material * material = nullptr;
};

#endif // !_MESHRENDERER_H_



