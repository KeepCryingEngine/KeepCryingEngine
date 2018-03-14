#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include <set>
#include <experimental/filesystem>

#include "Component.h"

class Animator : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Animator;

	Animator();
	virtual ~Animator();

	virtual void Start() override;
	virtual void RealUpdate() override;

	void DrawUI() override;

	virtual std::vector<Component::Type> GetNeededComponents() const override;
	virtual std::vector<Component::Type> GetProhibitedComponents() const override;

	unsigned int GetAnimInstanceId() const;
	
	void LoadAnimInstance(const std::experimental::filesystem::path& path);
	void PlayAnimInstance(const char* name);

	bool HasValidAnimationInstance() const;

	virtual void Load(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	static void UpdateTransformRecursive(unsigned int animInstanceId, GameObject* gameObject);

private:
	unsigned int animInstanceId = -1;
	std::string currentAnimationName;
	std::set<std::string> animationNames;
	std::set<std::experimental::filesystem::path> animationPaths;
};

#endif