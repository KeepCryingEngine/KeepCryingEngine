#include "AudioSource.h"

#include <bass.h>
#include <imgui.h>

#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "Transform.h"

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
	if(audioInfo == nullptr)
	{
		return;
	}

	if(reloadId)
	{
		switch(audioInfo->type)
		{
			case SoundType::MUSIC:
			{
				id = BASS_SampleGetChannel(App->audio->GetMusic(audioInfo->id,mode), FALSE);
			}
			break;
			case SoundType::SFX:
			{
				id = App->audio->GetSFX(audioInfo->id,mode);
			}
			break;
			default:
				assert(false);
				break;
		}
		reloadId = false;
	}

	if(id == 0)
	{
		return;
	}
	//Set audio properties
	BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, volume);
	BASS_ChannelSetAttribute(id, BASS_ATTRIB_PAN,pan);
	BASS_ChannelSetAttribute(id, BASS_ATTRIB_MUSIC_SPEED, pitch);
	BASS_Set3DFactors(0,rollOffFactor,doplerFactor);
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

			BASS_ChannelSet3DAttributes(id, BASS_3DMODE_NORMAL, 0, maxDistance, -1, -1, -1);

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

void AudioSource::DrawUI()
{
	if(ImGui::CollapsingHeader("Audio Source"))
	{
		static char pathToAudio[180] = "";

		ImGui::InputText("##PathToAudio", pathToAudio, sizeof(pathToAudio)); ImGui::SameLine();
		if(ImGui::Button("Load audio"))
		{
			std::experimental::filesystem::path path(pathToAudio);
			Load(path);
		}

		static bool isLooping = GetLoop();
		if(ImGui::Checkbox(" Frustum Culling", &isLooping))
		{
			SetLoop(isLooping);
		}

		int audioMode = (int)mode;
		if(ImGui::Combo("Mode", &audioMode, "Stereo\0Mono"))
		{
			mode = (SoundProperty)audioMode;
		}

		static float volume = GetVolume();
		if(ImGui::DragFloat("Volume", &volume,0.05f,0.0f,1.0f))
		{
			SetVolume(volume);
		}

		static float pitch = GetPitch();
		if(ImGui::DragFloat("Pitch", &pitch,1.0f,0.0f,255.0f))
		{
			SetPitch(pitch);
		}

		static float pan = GetPan();
		if(ImGui::DragFloat("Pan", &pan,0.05f,-1.0f,1.0f))
		{
			SetPan(pan);
		}

		static float maxDistance = GetMaxDistance();
		if(ImGui::DragFloat("Max Distance", &maxDistance,1.0f,1.0f,10000.0f))
		{
			SetMaxDistance(maxDistance);
		}

		static float rollOff = GetRollOffFactor();
		if(ImGui::DragFloat("RollOff factor", &rollOff,0.1f,0.0f,10.0f))
		{
			SetRollOffFactor(rollOff);
		}

		static float dopler = GetDoplerFactor();
		if(ImGui::DragFloat("Dopler factor", &dopler,0.1f,0.0f,10.0f))
		{
			SetDoplerFactor(dopler);
		}

		if(ImGui::Button("Play"))
		{			
			if(state == SourceStates::PAUSED)
			{
				state = SourceStates::WAITING_TO_UNPAUSE;
			}
			else if(state != SourceStates::PLAYING)
			{
				state = SourceStates::WAITING_TO_PLAY;
				reloadId = true;
			}
		}ImGui::SameLine();
		if(ImGui::Button("Pause"))
		{
			if(state != SourceStates::PAUSED)
			{
				state = SourceStates::WAITING_TO_PAUSE;
			}
		}ImGui::SameLine();
		if(ImGui::Button("Stop"))
		{
			if(state != SourceStates::STOPPED)
			{
				state = SourceStates::WAITING_TO_STOP;
			}
		}
	}
}

void AudioSource::SetMusic(AudioId* audioInfo)
{
	this->audioInfo = audioInfo;
}

void AudioSource::SetMode(SoundProperty newMode)
{
	mode = newMode;
}

void AudioSource::SetVolume(float value)
{
	volume = value;
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

void AudioSource::SetLoop(bool value)
{
	loop = value;
}

AudioId* AudioSource::GetMusic() const
{
	return audioInfo;
}

SoundProperty AudioSource::GetMode() const
{
	return mode;
}

float AudioSource::GetVolume() const
{
	return volume;
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

bool AudioSource::GetLoop()
{
	return loop;
}

void AudioSource::Load(const std::experimental::filesystem::path & path)
{

	AudioId* temp = App->audio->Load(path);
	if(temp == nullptr)
	{
		LOG_DEBUG("Error on audio load");
		assert(false);
		return;
	}
	BASS_ChannelStop(id);
	state = SourceStates::STOPPED;

	audioInfo = temp;
	reloadId = true;
}

