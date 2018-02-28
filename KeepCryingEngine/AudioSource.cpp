#include "AudioSource.h"

AudioSource::AudioSource():Component(ComponentType::AudioSource)
{}

AudioSource::~AudioSource()
{}

void AudioSource::Awake()
{}

void AudioSource::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{}

void AudioSource::DrawUI()
{}

void AudioSource::SetMusic(MusicId id)
{
	this->id = id;
}

void AudioSource::SetMode(AudioMode newMode)
{
	mode = newMode;
}

void AudioSource::SetPitch(float value)
{
	pitch = value;
}

void AudioSource::SetPan(float value)
{
	pan = value;
}

void AudioSource::SetMaxDistance(float value)
{
	maxDistance = value;
}

void AudioSource::SetRollOffFactor(float value)
{
	rollOffFactor = value;
}

void AudioSource::SetDoplerFactor(float value)
{
	doplerFactor = value;
}

MusicId AudioSource::GetMusic() const
{
	return id;
}

AudioMode AudioSource::GetMode() const
{
	return mode;
}

float AudioSource::GetPitch() const
{
	return pitch;
}

float AudioSource::GetPan() const
{
	return pan;
}

float AudioSource::GetMaxDistance() const
{
	return maxDistance;
}

float AudioSource::GetRollOffFactor() const
{
	return rollOffFactor;
}

float AudioSource::GetDoplerFactor() const
{
	return doplerFactor;
}
