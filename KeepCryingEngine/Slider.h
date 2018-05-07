#ifndef _SLIDER_H_
#define  _SLIDER_H_

#include "Component.h"
#include <btBulletDynamicsCommon.h>

class Slider : public Component
{
public:
	static const Component::Type TYPE = Component::Type::Slider;
	Slider();
	~Slider();

	void RealUpdate()override;

	virtual void DrawUI() override;

	virtual void PreLoad(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

	void ApplyConstraint(btRigidBody& ownBody, btRigidBody& secondBody);

private:
	btSliderConstraint * constraint = nullptr;

	float3 ownPivot = float3::zero;
	float3 ownAngle = float3::one;

	float3 secondPivot = float3::zero;
	float3 secondAngle = float3::one;
};

#endif // !_SLIDER_H_

