#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include <bass.h>
#include <map>
#include <vector>
#include <list>
#include <experimental/filesystem>

#include "Module.h"

class AudioClip;
class AudioSource;
class AudioListener;
class SoundEffects;

typedef unsigned MusicId;

enum class AudioType {
	SFX,
	Music
};

enum class ChannelType
{
	Mono,
	Stereo
};

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init() override;
	bool CleanUp() override;

	AudioClip* Load(const std::experimental::filesystem::path& path, AudioType type, ChannelType channelType);

	void EnableListener(AudioListener* listener);

	AudioListener* GetActiveListener() const;

	SoundEffects* GetSceneEffects()const;

	void SubscribeSource(AudioSource& source);
	void Unsubscribe(AudioSource& source);

	const std::vector<AudioSource*>& GetAllAudioSources() const;

private:
	AudioClip* LoadMusic(const std::experimental::filesystem::path& path, ChannelType channelType);
	AudioClip* LoadSFX(const std::experimental::filesystem::path& path, ChannelType channelType);

private:
	std::map<std::string, AudioClip*> soundCache;

	AudioListener* activeListener = nullptr;

	SoundEffects* sceneEffects = nullptr;

	std::vector<AudioSource*> sceneSources;

	static const int DEVICE;
	static const int FRECUENCY;
};

#endif // !_MODULEAUDIO_H_

