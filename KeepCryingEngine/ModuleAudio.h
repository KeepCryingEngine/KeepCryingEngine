#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include <bass.h>
#include <map>
#include <vector>
#include <list>
#include <experimental/filesystem>

#include "Module.h"
#include "AssetManager.h"
#include "AudioClip.h"

class AudioSource;
class AudioListener;
class SoundEffects;

class ModuleAudio : public AssetManager<AudioClipIdentifier, AudioClip>
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init() override;
	bool CleanUp() override;

	void EnableListener(AudioListener* listener);

	AudioListener* GetActiveListener() const;

	SoundEffects* GetSceneEffects()const;

	void SubscribeSource(AudioSource& source);
	void Unsubscribe(AudioSource& source);

	const std::vector<AudioSource*>& GetAllAudioSources() const;

protected:
	virtual AudioClip * Load(const AudioClipIdentifier & identifier) override;
	virtual void Unload(AudioClip * asset) override;

private:
	AudioClip* LoadMusic(const std::experimental::filesystem::path& path, ChannelType channelType);
	AudioClip* LoadSFX(const std::experimental::filesystem::path& path, ChannelType channelType);

private:
	AudioListener* activeListener = nullptr;
	SoundEffects* sceneEffects = nullptr;
	std::vector<AudioSource*> sceneSources;

	static const int DEVICE;
	static const int FRECUENCY;

};

#endif // !_MODULEAUDIO_H_

