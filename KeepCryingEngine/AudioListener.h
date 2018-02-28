#ifndef _AUDIOLISTENER_H_
#define _AUDIOLISTENER_H_

#include "Component.h"

class AudioListener : public Component
{
public:
	AudioListener();
	~AudioListener();

	void Awake()override;
	void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;

	void DrawUI() override;
};

#endif // !_AUDIOLISTENER_H_

