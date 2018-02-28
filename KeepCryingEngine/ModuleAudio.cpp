#include "ModuleAudio.h"
#include <algorithm>

const int ModuleAudio::DEVICE = -1; // Default Sounddevice
const int ModuleAudio::FRECUENCY = 44100;
MusicId ModuleAudio::sfxActualIndex = 0;
MusicId ModuleAudio::musicActualIndex = 0;

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
		HSTREAM streamHandle = BASS_StreamCreateFile(FALSE, "your_file.mp3", 0, 0, 0);
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
		HCHANNEL streamHandle = BASS_SampleLoad(TRUE, (path + name +"."+extension).c_str(),0 , 0, 0, 0);
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
	return sfx[id];
}

HCHANNEL ModuleAudio::GetMusic(MusicId id) const
{
	return music[id];
}
