#include "AudioSource.h"

#include <bass.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "Transform.h"



AudioSource::AudioSource():Component(AudioSource::TYPE)
{}

AudioSource::~AudioSource()
{}

void AudioSource::Awake()
{}

void AudioSource::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	unsigned long id = 0;
	switch(audioInfo.type)
	{
		case SoundType::MUSIC:
		{
			id= BASS_SampleGetChannel(App->audio->GetMusic(audioInfo.id),FALSE);
		}
		break;
		case SoundType::SFX:
		{
			id = App->audio->GetSFX(audioInfo.id);
		}
		break;
		default:
			assert(false);
			break;
	}

	if(id = 0)
	{
		return;
	}

	switch(state)
	{
		case SourceStates::PLAYING:
		{
			BASS_ChannelSet3DAttributes(id, BASS_3DMODE_NORMAL, 0, maxDistance, -1, -1, -1);

			// Update 3D position

			Transform* body = gameObject->GetTransform();

			BASS_ChannelSet3DPosition(id,
				(BASS_3DVECTOR*)&body->GetWorldPosition(), // position
				(BASS_3DVECTOR*)&body->Forward(), // front
				nullptr); // velocity
		}
			break;
		case SourceStates::WAITING_TO_PLAY:
		{
			if(BASS_ChannelPlay(id, FALSE) == FALSE)
			{
				//LOG_DEBUG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, 0.0f);
				BASS_ChannelSlideAttribute(id, BASS_ATTRIB_VOL, 1.0f, DWORD(fadeIn * 1000.0f));
				state = SourceStates::PLAYING;
			}
		}
			break;
		case SourceStates::WAITING_TO_STOP:
		{
			if(BASS_ChannelStop(id) == FALSE)
			{
				//LOG("BASS_ChannelStop() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				BASS_ChannelSlideAttribute(id, BASS_ATTRIB_VOL, 0.0f, DWORD(fadeOut * 1000.0f));
				state = SourceStates::STOPPED;
			}
		}
			break;
		case SourceStates::WAITING_TO_PAUSE:
		{
			if(BASS_ChannelPause(id) == FALSE)
			{
				//LOG("BASS_ChannelPause() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				state = SourceStates::PAUSED;
			}
		}
			break;
		case SourceStates::WAITING_TO_UNPAUSE:
		{
			if(BASS_ChannelPlay(id, FALSE) == FALSE)
			{
				//LOG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				state = SourceStates::PLAYING;
			}
		}
			break;
		default:
			assert(false);
			break;
	}
}

void AudioSource::DrawUI()
{}

void AudioSource::SetMusic(AudioId audioInfo)
{
	this->audioInfo = audioInfo;
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

AudioId AudioSource::GetMusic() const
{
	return audioInfo;
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
