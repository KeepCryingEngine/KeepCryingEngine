#include "SoundsEffects.h"

#include "Application.h"
#include "ModuleAudio.h"
#include "AudioSource.h"

using namespace std;
SoundEffects::SoundEffects()
{}

SoundEffects::~SoundEffects()
{}

void SoundEffects::AddEffect(EffectInfo& effect)
{
	effectsList.push_back(&effect);
	Callback();
}

void SoundEffects::RemoveEffect(EffectInfo& effect)
{
	list<EffectInfo*>::iterator it = find(effectsList.begin(), effectsList.end(), &effect);
	if(it != effectsList.end())
	{
		effectsList.erase(it);
		Callback();
	}
}

//TODO: add and remove more than 1 effect at time(If needed)

const std::list<EffectInfo*>& SoundEffects::GetEffects()
{
	return effectsList;
}

void SoundEffects::Callback()
{
	vector<AudioSource*> sources = App->audio->GetAllAudioSources();

	for(AudioSource* audioSource : sources)
	{
		if(audioSource->id != 0)
		{
			audioSource->UpdateChannelEffects();
		}
	}
}
