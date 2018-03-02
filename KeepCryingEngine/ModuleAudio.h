#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include <bass.h>
#include <map>
#include <vector>
#include <list>

#include "Module.h"

class AudioListener;

typedef unsigned MusicId;

enum class SoundType
{
	SFX,
	MUSIC
};

enum class SoundProperty
{
	STEREO,
	MONO
};

struct AudioId
{
	SoundType type;
	MusicId id;
};

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init() override;
	bool CleanUp() override;
	update_status PostUpdate(float deltaTimeS, float realDeltaTimeS) override;

	AudioId* Load(const std::string& path, const std::string& name, const std::string& extension);
	HSTREAM GetSFX(MusicId id, SoundProperty p)const;
	HSAMPLE GetMusic(MusicId id, SoundProperty p) const;

	void EnableListener(AudioListener* listener);

	AudioListener* GetActiveListener()const;
private:
	bool LoadOgg(const std::string& path, const std::string& name, const std::string& extension, AudioId& audio);
	bool LoadWav(const std::string& path, const std::string& name, const std::string& extension, AudioId& audio);

private:
	static MusicId sfxActualIndex;
	static MusicId musicActualIndex;
	//Stereo
	std::map<MusicId,HSTREAM> stereoSfx;
	
	std::map<MusicId, HSAMPLE> stereoMusic;
	
	//Mono
	std::map<MusicId, HSTREAM> monoSfx;
	std::map<MusicId, HSAMPLE> monoMusic;

	//Common
	std::list<MusicId> musicHoles;
	std::list<MusicId> sfxHoles;

	std::map<std::string, AudioId> soundCache;

	AudioListener* activeListener = nullptr;

	static const int DEVICE;
	static const int FRECUENCY;
};

#endif // !_MODULEAUDIO_H_

