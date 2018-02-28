#include "ModuleAudio.h"
#include <algorithm>

#include "AudioListener.h"

const int ModuleAudio::DEVICE = -1; // Default Sounddevice
const int ModuleAudio::FRECUENCY = 44100;
MusicId ModuleAudio::sfxActualIndex = 1;
MusicId ModuleAudio::musicActualIndex = 1;

using namespace std;

ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{}

bool ModuleAudio::Init()
{
	BASS_Init(DEVICE, FRECUENCY, BASS_DEVICE_3D, 0, NULL);
	return true;
}

bool ModuleAudio::CleanUp()
{
	return true;
}

update_status ModuleAudio::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{
	return update_status::UPDATE_CONTINUE;
}

AudioId* ModuleAudio::Load(const string & path, const string & name, const string & extension)
{
	if(soundCache.count(path + name + "." + extension))
	{
		return &soundCache[path + name + "." + extension];
	}

	AudioId* newAudioId = new AudioId();
	if(extension == "ogg")
	{
		HSTREAM streamHandle = BASS_StreamCreateFile(FALSE, (path + name + "." + extension).c_str(), 0, 0, BASS_SAMPLE_3D);
		if(streamHandle == 0)
		{
			int a = BASS_ErrorGetCode();
			LOG_DEBUG("Error loading ogg file");
			assert(false);
		}
		MusicId newId;
		if(sfxHoles.size() > 0)
		{
			newId = sfxHoles.front();
			sfxHoles.pop_front();
		}
		else
		{
			newId = sfxActualIndex;
			sfxActualIndex++;
		}
		sfx[newId] = streamHandle;
		newAudioId->id = newId;
		newAudioId->type = SoundType::SFX;
	}
	else if(extension == "wav")
	{
		HSAMPLE streamHandle = BASS_SampleLoad(FALSE, (path + name +"."+extension).c_str(),0 ,0 ,5,BASS_SAMPLE_3D|| BASS_SAMPLE_OVER_VOL);
		if(streamHandle == 0)
		{
			int a = BASS_ErrorGetCode();
			LOG_DEBUG("Error loading wav file");
			assert(false);
		}
		MusicId newId;
		if(musicHoles.size() > 0)
		{
			newId = musicHoles.front();
			musicHoles.pop_front();
		}
		else
		{
			newId = musicActualIndex;
			musicActualIndex++;
		}
		music[newId] = streamHandle;
		newAudioId->id = newId;
		newAudioId->type = SoundType::MUSIC;
	}
	else
	{
		return nullptr;
	}
	soundCache[path + name + "." + extension] = *newAudioId;
	return newAudioId;
}

HSTREAM ModuleAudio::GetSFX(MusicId id) const
{
	return sfx.at(id);
}

HCHANNEL ModuleAudio::GetMusic(MusicId id) const
{
	return music.at(id);
}

void ModuleAudio::EnableListener(AudioListener* listener)
{
	if(activeListener != nullptr)
	{
		activeListener->enabled = false;
	}

	if(listener != nullptr)
	{
		listener->enabled = true;
	}

	activeListener = listener;
}

AudioListener * ModuleAudio::GetActiveListener() const
{
	return activeListener;
}
