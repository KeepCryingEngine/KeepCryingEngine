#include "AudioListener.h"
#include <bass.h>

#include "GameObject.h"
#include "Transform.h"

AudioListener::AudioListener():Component(ComponentType::AudioListener)
{}

AudioListener::~AudioListener()
{}

void AudioListener::Awake()
{}

void AudioListener::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	Transform* body = gameObject->GetTransform();
	BASS_Set3DPosition(

		(BASS_3DVECTOR*)  &body->GetWorldPosition(), // position

		nullptr, // speed

		(BASS_3DVECTOR*) &body->GetFront(), // front

		(BASS_3DVECTOR*) &body->GetUp()); // up}
}

void AudioListener::DrawUI()
{}
