#include "AudioListener.h"
#include <bass.h>

#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModuleAudio.h"

AudioListener::AudioListener():Component(AudioListener::TYPE)
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

		(BASS_3DVECTOR*) &body->Forward(), // front

		(BASS_3DVECTOR*) &body->Up()); // up}
}

void AudioListener::SetEnable(bool enable)
{
	if(enable)
	{
		enabled = wasEnabled;
	}
	else
	{
		enabled = enable;
	}

	App->audio->EnableListener(enabled ? this : nullptr);
}

void AudioListener::DrawUI()
{}
