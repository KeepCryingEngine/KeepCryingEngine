#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

#include "Component.h"

class Animator : public Component
{
public:
	static const ComponentType TYPE = ComponentType::Animator;

	Animator();
	virtual ~Animator();

	virtual void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;

	void DrawUI() override;

	virtual std::vector<ComponentType> GetNeededComponents() const override;
	virtual std::vector<ComponentType> GetProhibitedComponents() const override;

	unsigned int GetAnimInstanceId() const;
	
	void SetAnimInstance(const char* path);
	void PlayAnimInstance(const char* name);

	bool HasValidAnimationInstance() const;

private:
	static void UpdateTransformRecursive(unsigned int animInstanceId, GameObject* gameObject);

private:
	unsigned int animInstanceId = -1;
};

#endif