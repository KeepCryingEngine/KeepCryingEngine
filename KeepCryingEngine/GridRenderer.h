#ifndef _GRIDRENDERER_H_
#define _GRIDRENDERER_H_

#include <vector>

#include "Billboard.h"
#include "Component.h"

class Material;

class GridRenderer : public Component
{
public:
	static const Component::Type TYPE = Component::Type::GridRenderer;

	GridRenderer();
	virtual ~GridRenderer();

	void DrawUI() override;

	std::vector<Component::Type> GetProhibitedComponents() const override;
	std::vector<Component::Type> GetNeededComponents() const override;

	void Render();

	void SetMaterial(Material& material);
	Material* GetMaterial() const;

	void Build(uint rows, uint columns, const float2& size);

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:

	void Clear();

private:
	Material* material = nullptr;

	uint rows = 0;
	uint columns = 0;

	float2 size = float2::zero;

	std::vector<Billboard*> billboards;
};

#endif