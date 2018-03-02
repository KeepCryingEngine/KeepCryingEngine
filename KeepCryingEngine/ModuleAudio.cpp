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
	stereoMusic.clear();
	stereoSfx.clear();
	monoMusic.clear();
	monoSfx.clear();
	return true;
}

update_status ModuleAudio::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{
	return update_status::UPDATE_CONTINUE;
}

AudioId* ModuleAudio::Load(const std::experimental::filesystem::path& path)
{
	if(soundCache.count(path.string()))
	{
		return &soundCache[path.string()];
	}

	AudioId* newAudioId = new AudioId();
	string extension = path.extension().string();
	if(extension == ".ogg")
	{
		if(!LoadOgg(path, *newAudioId))
		{
			return nullptr;
		}
	}
	else if(extension == ".wav")
	{
		if(!LoadWav(path, *newAudioId))
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
	soundCache[path.string()] = *newAudioId;
	return newAudioId;
}

HSTREAM ModuleAudio::GetSFX(MusicId id, SoundProperty p) const
{
	HSTREAM ret = 0;
	switch(p)
	{
		case SoundProperty::STEREO:
		{
			ret = stereoSfx.at(id);
		}
			break;
		case SoundProperty::MONO:
		{
			ret = monoSfx.at(id);
		}
			break;
		default:
			assert(false);
			break;
	}

	return ret;
}

HCHANNEL ModuleAudio::GetMusic(MusicId id, SoundProperty p) const
{
	HCHANNEL ret = 0;
	switch(p)
	{
		case SoundProperty::STEREO:
		{
			ret = stereoMusic.at(id);
		}
			break;
		case SoundProperty::MONO:
		{
			ret = monoMusic.at(id);
		}
			break;
		default:
			assert(false);
			break;
	}
	return ret;
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

bool ModuleAudio::LoadOgg(const std::experimental::filesystem::path& path, AudioId& audio)
{
	HSTREAM streamHandleStereo = BASS_StreamCreateFile(FALSE, path.string().c_str(), 0, 0, BASS_SAMPLE_3D);
	if(streamHandleStereo == 0)
	{
		LOG_DEBUG("Error loading ogg file");
		assert(false);
		return false;
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
	stereoSfx[newId] = streamHandleStereo;
	audio.id = newId;
	audio.type = SoundType::SFX;

	//Mono
	HSTREAM streamHandleMono = BASS_StreamCreateFile(FALSE, path.string().c_str(), 0, 0, BASS_SAMPLE_MONO | BASS_SAMPLE_3D);
	if(streamHandleMono == 0)
	{
		LOG_DEBUG("Error loading ogg file");
		assert(false);
		return false;
	}
	monoSfx[newId] = streamHandleMono;
	return true;
}

bool ModuleAudio::LoadWav(const std::experimental::filesystem::path& path, AudioId& audio)
{
	//Stereo
	HSAMPLE streamHandleStereo = BASS_SampleLoad(FALSE, path.string().c_str(), 0, 0, 5, BASS_SAMPLE_3D | BASS_SAMPLE_OVER_VOL);
	if(streamHandleStereo == 0)
	{
		LOG_DEBUG("Error loading wav file");
		assert(false);
		return false;
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
	stereoMusic[newId] = streamHandleStereo;
	audio.id = newId;
	audio.type = SoundType::MUSIC;

	//Mono
	HSAMPLE streamHandleMono = BASS_SampleLoad(FALSE, path.string().c_str(), 0, 0, 5, BASS_SAMPLE_MONO | BASS_SAMPLE_3D | BASS_SAMPLE_OVER_VOL);
	if(streamHandleMono == 0)
	{
		LOG_DEBUG("Error loading wav file");
		assert(false);
		return false;
	}
	monoMusic[newId] = streamHandleMono;
	return true;
}
