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
}

AudioSource::~AudioSource()
{}

void AudioSource::Awake()
{}

void AudioSource::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	if(audioClip == nullptr)
	{
		return;
	}

	if(id == 0)
	{
		return;
	}

	//Set audio properties
	BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, volume);
	BASS_ChannelSetAttribute(id, BASS_ATTRIB_PAN,pan);
	BASS_ChannelSetAttribute(id, BASS_ATTRIB_FREQ, originalFreq + freqModifier);
	BASS_Set3DFactors(1.0f, rollOffFactor, doplerFactor);

	BASS_Apply3D();

	//LOOPcontrol
	if(loop)
	{
		BASS_ChannelFlags(id, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
	}
	else
	{
		BASS_ChannelFlags(id, 0, BASS_SAMPLE_LOOP);
	}

	switch(state)
	{
		case SourceStates::PLAYING:
		{
			if(BASS_ChannelIsActive(id) == BASS_ACTIVE_STOPPED && !loop)
			{
				state = SourceStates::WAITING_TO_STOP;
				break;
			}

			BASS_ChannelSet3DAttributes(id, BASS_3DMODE_NORMAL, minDistance, maxDistance, 360, 360, -1);
			BASS_Apply3D();

			// Update 3D position

			Transform* body = gameObject->GetTransform();
	
			BASS_ChannelSet3DPosition(id,
				(BASS_3DVECTOR*)&body->GetWorldPosition(), // position
				(BASS_3DVECTOR*)&body->Forward(), // front
				nullptr); // velocity


			BASS_Apply3D();
		}
			break;
		case SourceStates::WAITING_TO_PLAY:
		{
			if(BASS_ChannelPlay(id, FALSE) == FALSE)
			{
				int a = BASS_ErrorGetCode();
				int espera = 0;
				//LOG_DEBUG("BASS_ChannelPlay() with channel [%ul] error: %s", id, BASS_GetErrorString());
			}
			else
			{
				BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, 0.0f);
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
	}
}

void AudioSource::UpdateChannelForAudio()
{
	switch (audioClip->type)
	{
	case AudioType::Music:
	{
		id = BASS_SampleGetChannel(audioClip->musicSample, FALSE);
	}
	break;
	case AudioType::SFX:
	{
		id = audioClip->sfxStream;
	}
	break;
	default:
		assert(false);
		break;
	}
}

void AudioSource::DrawUI()
{
	if(ImGui::CollapsingHeader("Audio Source"))
	{
		static char pathToAudio[180] = "Assets/sfx/oggSound.ogg";

		ImGui::InputText("##PathToAudio", pathToAudio, sizeof(pathToAudio)); 
		
		int channelType = (int)loadingChannelType;
		if (ImGui::Combo("ChannelType", &channelType, "Mono\0Stereo"))
		{
			loadingChannelType = (ChannelType)channelType;
		}

		int audioType = (int)loadingAudioType;
		if (ImGui::Combo("AudioType", &audioType, "SFX\0Music"))
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
		ImGui::DragFloat("Pan", &pan, 0.05f, -1.0f, 1.0f);
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
		UpdateChannelForAudio();
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

bool AudioSource::GetLoop()
{
	return loop;
}

void AudioSource::OnLoadButtonPressed(const std::experimental::filesystem::path & path)
{
	AudioClip* audioClip = App->audio->Load(path, loadingAudioType ,loadingChannelType);
	if(audioClip != nullptr)
	{
		BASS_ChannelStop(id);
		state = SourceStates::STOPPED;

		this->audioClip = audioClip;
		UpdateChannelForAudio();

		BASS_ChannelGetAttribute(id, BASS_ATTRIB_FREQ, &originalFreq);
	}
}

