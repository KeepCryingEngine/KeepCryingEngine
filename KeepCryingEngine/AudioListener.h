#ifndef _AUDIOLISTENER_H_
#define _AUDIOLISTENER_H_

#include "Component.h"

class AudioListener : public Component
{
public:
	static const Component::Type TYPE = Component::Type::AudioListener;
	AudioListener();
	~AudioListener();

	void Awake()override;
	void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;

	void SetEnable(bool enable)override;

	void DrawUI() override;
};

#endif // !_AUDIOLISTENER_H_

