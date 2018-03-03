#include "ModuleAudio.h"
#include <algorithm>

#include "AudioListener.h"
#include "AudioClip.h"
#include "AudioSource.h"

const int ModuleAudio::DEVICE = -1; // Default Sounddevice
const int ModuleAudio::FRECUENCY = 44100;

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

AudioClip* ModuleAudio::Load(const std::experimental::filesystem::path& path, AudioType type, ChannelType channelType)
{
	if(soundCache.count(path.string()))
	{
		return soundCache[path.string()];
	}

	AudioClip* audioClip = nullptr;
	string extension = path.extension().string();

	switch (type)
	{
	case AudioType::Music:
		audioClip = LoadMusic(path, channelType);
		break;
	case AudioType::SFX:
		audioClip = LoadSFX(path, channelType);
		break;
	}
	
	soundCache[path.string()] = audioClip;

	return audioClip;
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

void ModuleAudio::SubscribeSource(AudioSource& source)
{
	//If subscribe item is not already in the list, add it
	if(find(sceneSources.begin(),sceneSources.end(),&source) == sceneSources.end())
	{
		sceneSources.push_back(&source);
	}
}

void ModuleAudio::Unsubscribe(AudioSource& source)
{
	vector<AudioSource*>::iterator it = find(sceneSources.begin(), sceneSources.end(), &source);
	if( it != sceneSources.end())
	{
		sceneSources.erase(it);
	}
}

const vector<AudioSource*>& ModuleAudio::GetAllAudioSources() const
{
	return sceneSources;
}

AudioClip * ModuleAudio::LoadMusic(const std::experimental::filesystem::path & path, ChannelType channelType)
{
	AudioClip* audioClip = nullptr;
	HSTREAM handle = 0;
	switch (channelType)
	{
	case ChannelType::Mono:
		handle = BASS_StreamCreateFile(FALSE, path.string().c_str(), 0, 0, BASS_SAMPLE_MONO | BASS_SAMPLE_3D);
		break;
	case ChannelType::Stereo:
		handle = BASS_StreamCreateFile(FALSE, path.string().c_str(), 0, 0, BASS_SAMPLE_3D);
		break;
	}

	if (handle != 0)
	{
		audioClip = new AudioClip();
		audioClip->type = AudioType::Music;
		audioClip->channelType = channelType;
		audioClip->musicStream = handle;
	}

	return audioClip;
}

AudioClip * ModuleAudio::LoadSFX(const std::experimental::filesystem::path & path, ChannelType channelType)
{
	AudioClip* audioClip = nullptr;
	HSAMPLE handle = 0;
	switch (channelType)
	{
	case ChannelType::Mono:
		handle = BASS_SampleLoad(FALSE, path.string().c_str(), 0, 0, 5, BASS_SAMPLE_MONO | BASS_SAMPLE_3D | BASS_SAMPLE_OVER_VOL);
		break;
	case ChannelType::Stereo:
		handle = BASS_SampleLoad(FALSE, path.string().c_str(), 0, 0, 5, BASS_SAMPLE_3D | BASS_SAMPLE_OVER_VOL);
		break;
	}
	
	if (handle != 0)
	{
		audioClip = new AudioClip();
		audioClip->type = AudioType::SFX;
		audioClip->channelType = channelType;
		audioClip->sfxSample = handle;
	}

	return audioClip;
}
