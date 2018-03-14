#include "AudioSource.h"

#include <bass.h>
#include <imgui.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "Transform.h"
#include "AudioClip.h"

using namespace std;

AudioSource::AudioSource():Component(AudioSource::TYPE)
{
	App->audio->SubscribeSource(*this);
}

AudioSource::~AudioSource()
{
	App->audio->Unsubscribe(*this);
}

void AudioSource::Awake()
{}

void AudioSource::RealUpdate()
{
	if(audioClip == nullptr)
	{
		return;
	}

	if(channel == 0)
	{
		return;
	}

	//Set audio properties
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_FREQ, originalFreq + freqModifier);
	BASS_Set3DFactors(1.0f, rollOffFactor, doplerFactor);
	BASS_Apply3D();

	//LOOPcontrol
	if(loop)
	{
		BASS_ChannelFlags(channel, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	}
	else
	{
		BASS_ChannelFlags(channel, 0, BASS_SAMPLE_LOOP);
	}

	switch(state)
	{
		case SourceStates::PLAYING:
		{
			if(BASS_ChannelIsActive(channel) == BASS_ACTIVE_STOPPED && !loop)
			{
				state = SourceStates::WAITING_TO_STOP;
				break;
			}

			BASS_ChannelSet3DAttributes(channel, BASS_3DMODE_NORMAL, minDistance, maxDistance, 360, 360, -1);
			BASS_Apply3D();

			// Update 3D position

			Transform* body = gameObject->GetTransform();
	
			BASS_ChannelSet3DPosition(channel,
				(BASS_3DVECTOR*)&body->GetWorldPosition(), // position
				(BASS_3DVECTOR*)&body->Forward(), // front
				(BASS_3DVECTOR*)&body->Velocity()); // velocity


			BASS_Apply3D();
		}
			break;
		case SourceStates::WAITING_TO_PLAY:
		{
			if(BASS_ChannelPlay(channel, FALSE))
			{
				BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, 0.0f);
				state = SourceStates::PLAYING;
			}
			else
			{
				//LOG_DEBUG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
		}
			break;
		case SourceStates::WAITING_TO_STOP:
		{
			ClearChannelEffects();
			if(BASS_ChannelStop(channel))
			{
				channel = 0;
				state = SourceStates::STOPPED;
			}
			else
			{
				//LOG("BASS_ChannelStop() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
		}
			break;
		case SourceStates::WAITING_TO_PAUSE:
		{
			if(BASS_ChannelPause(channel) == FALSE)
			{
				state = SourceStates::PAUSED;
			}
			else
			{
				//LOG("BASS_ChannelPause() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
		}
			break;
		case SourceStates::WAITING_TO_UNPAUSE:
		{
			if(BASS_ChannelPlay(channel, FALSE) == FALSE)
			{
				state = SourceStates::PLAYING;
			}
			else
			{
				//LOG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
		}
			break;
	}
}

void AudioSource::DrawUI()
{
	if(ImGui::CollapsingHeader("Audio Source"))
	{
		ImGui::Checkbox("Active", &enabled); ImGui::SameLine();
		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}
		static char pathToAudio[180] = "Assets/sfx/oggSound.ogg";

		ImGui::InputText("##PathToAudio", pathToAudio, sizeof(pathToAudio));

		int channelType = (int)loadingChannelType;
		if(ImGui::Combo("ChannelType", &channelType, "Mono\0Stereo"))
		{
			loadingChannelType = (ChannelType)channelType;
		}

		int audioType = (int)loadingAudioType;
		if(ImGui::Combo("AudioType", &audioType, "SFX\0Music"))
		{
			loadingAudioType = (AudioType)audioType;
		}

		if(ImGui::Button("Load audio"))
		{
			std::experimental::filesystem::path path(pathToAudio);
			OnLoadButtonPressed(path);
		}


		ImGui::Checkbox(" Loop", &loop);
		ImGui::DragFloat("Volume", &volume, 0.05f, 0.0f, 1.0f);
		ImGui::DragFloat("Pitch", &freqModifier, 200.0f, -20000.0f, 60000.0f);
		ImGui::DragFloat("Min Distance", &minDistance, 0.5f, 0.0f, 10000.0f);
		ImGui::DragFloat("Max Distance", &maxDistance, 0.5f, 1.0f, 10000.0f);
		ImGui::DragFloat("RollOff factor", &rollOffFactor, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Dopler factor", &doplerFactor, 0.1f, 0.0f, 10.0f);


		if(ImGui::Button("Play"))
		{
			OnPlayButtonPressed();
		}

		ImGui::SameLine();

		if(ImGui::Button("Pause"))
		{
			OnPauseButtonPressed();
		}

		ImGui::SameLine();

		if(ImGui::Button("Stop"))
		{
			OnStopButtonPressed();
		}	
	}
}

void AudioSource::OnStopButtonPressed()
{
	if (state != SourceStates::STOPPED)
	{
		state = SourceStates::WAITING_TO_STOP;
	}
}

void AudioSource::OnPauseButtonPressed()
{
	if (state != SourceStates::PAUSED)
	{
		state = SourceStates::WAITING_TO_PAUSE;
	}
}

void AudioSource::OnPlayButtonPressed()
{
	if (state == SourceStates::PAUSED)
	{
		state = SourceStates::WAITING_TO_UNPAUSE;
	}
	else if (state != SourceStates::PLAYING)
	{
		state = SourceStates::WAITING_TO_PLAY;
		channel = GetChannelForAudio(audioClip);
		UpdateChannelEffects();
	}
}

DWORD AudioSource::GetChannelForAudio(const AudioClip* audioClip) const
{
	DWORD channel = 0;

	if (audioClip != nullptr)
	{
		switch (audioClip->type)
		{
		case AudioType::Music:
		{
			channel = audioClip->musicStream;
		}
		break;
		case AudioType::SFX:
		{
			channel = BASS_SampleGetChannel(audioClip->sfxSample, FALSE);
		}
		break;
		default:
			assert(false);
			break;
		}
	}
	return channel;
}

void AudioSource::UpdateChannelEffects()
{
	ClearChannelEffects();

	SoundEffects* newEffects = App->audio->GetSceneEffects();
	for(EffectInfo* ef:newEffects->GetEffects())
	{
		HFX reverbEffect = BASS_ChannelSetFX(channel, BASS_FX_DX8_I3DL2REVERB, ef->priority);
		BASS_FXSetParameters(reverbEffect, ef->reverbConfig);
		activeEffects.push_back(reverbEffect);
	}
}

void AudioSource::SetMusic(AudioClip* audioInfo)
{
	this->audioClip = audioInfo;
}

void AudioSource::SetVolume(float value)
{
	volume = value;
}

void AudioSource::SetPitch(float value)
{
	freqModifier = value;
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

void AudioSource::SetLoop(bool value)
{
	loop = value;
}

AudioClip* AudioSource::GetMusic() const
{
	return audioClip;
}

float AudioSource::GetVolume() const
{
	return volume;
}

float AudioSource::GetPitch() const
{
	return freqModifier;
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

bool AudioSource::GetLoop()
{
	return loop;
}

void AudioSource::Load(const nlohmann::json& json)
{
	volume = json["volume"];
	originalFreq = json["originalFreq"];
	freqModifier = json["freqModifier"];
	minDistance = json["minDistance"];
	maxDistance = json["maxDistance"];
	rollOffFactor = json["rollOffFactor"];
	doplerFactor = json["doplerFactor"];
	loop = json["loop"];
	path = json["path"];
	usedAudioType = json["usedAudioType"];
	usedChannelType = json["usedChannelType"];
}

void AudioSource::Save(nlohmann::json& json) const
{
	/*

	Relevant information:

	type
	volume
	originalFreq
	freqModifier
	minDistance
	maxDistance
	rollOffFactor
	doplerFactor
	loop
	path
	usedAudioType
	usedChannelType

	*/

	json["type"] = type;
	json["volume"] = volume;
	json["originalFreq"] = originalFreq;
	json["freqModifier"] = freqModifier;
	json["minDistance"] = minDistance;
	json["maxDistance"] = maxDistance;
	json["rollOffFactor"] = rollOffFactor;
	json["doplerFactor"] = doplerFactor;
	json["loop"] = loop;
	json["path"] = path.string();
	json["usedAudioType"] = usedAudioType;
	json["usedChannelType"] = usedChannelType;
}

void AudioSource::OnLoadButtonPressed(const std::experimental::filesystem::path & path)
{
	this->path = path;
	usedAudioType = loadingAudioType;
	usedChannelType = loadingChannelType;

	//TODO: Delete old audio clip if already loaded
	AudioClip* audioClip = App->audio->Load(path, loadingAudioType ,loadingChannelType);
	if(audioClip != nullptr)
	{
		BASS_ChannelStop(channel);

		this->audioClip = audioClip;

		BASS_ChannelGetAttribute(channel, BASS_ATTRIB_FREQ, &originalFreq);
	}
}

void AudioSource::ClearChannelEffects()
{
	for(HFX ef : activeEffects)
	{
		BASS_ChannelRemoveFX(channel, ef);
	}
	activeEffects.clear();
}