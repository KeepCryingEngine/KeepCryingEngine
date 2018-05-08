#ifndef _MESHFILTER_H_
#define _MESHFILTER_H_

#include "Component.h"

class Mesh;

enum class MeshMode
{
	CUBE,
	SPHERE,
	PLANE
};

class MeshFilter : public Component
{
public:
	static const Component::Type TYPE = Component::Type::MeshFilter;

	MeshFilter();
	~MeshFilter();

	void DrawUI() override;
	std::vector<Component::Type> GetProhibitedComponents() const override;
	std::vector<Component::Type> GetNeededComponents() const override;

	void RealUpdate() override;

	Mesh* GetMesh() const;
	void SetMesh(Mesh* mesh);
	void SetMeshMode(MeshMode mode);

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	void UpdateGameObjectAABB();
	void DrawGameObjectAABB();

private:
	Mesh * mesh = nullptr;

	MeshMode mode = MeshMode::CUBE;
};

#endif // !_MESHFILTER_H_




