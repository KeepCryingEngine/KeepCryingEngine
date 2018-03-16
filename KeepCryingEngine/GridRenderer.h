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

	void Start() override;

	void RealUpdate() override;
	void DrawUI() override;

	std::vector<Component::Type> GetProhibitedComponents() const override;
	std::vector<Component::Type> GetNeededComponents() const override;

	void Render();

	void SetMaterial(Material& material);
	Material* GetMaterial() const;

	void Build(int rows, int columns, const float2& size);

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	void UpdateBillboards();
	void Clear();

private:
	Material* material = nullptr;

	int rows = 1;
	int columns = 1;

	float2 size = float2::one;

	int numVertices = 0;
	int numIndices = 0;
	GLuint vertexPosBufferId = 0;
	GLuint vertexUvBufferId = 0;
	GLuint indicesBufferId = 0;

	std::vector<Billboard*> billboards;
	std::vector<float3> vertexPos;
	std::vector<float2> vertexUv;
	std::vector<GLushort> indices;
};

#endif