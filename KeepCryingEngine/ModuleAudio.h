#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include <bass.h>
#include <map>
#include <vector>
#include <list>

#include "Module.h"

typedef unsigned MusicId;

enum class SoundType
{
	SFX,
	MUSIC
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
	HSTREAM GetSFX(MusicId id)const;
	HCHANNEL GetMusic(MusicId id) const;

private:
	static MusicId sfxActualIndex;
	static MusicId musicActualIndex;
	std::vector<HSTREAM> sfx;
	std::list<MusicId> sfxHoles;
	std::vector<HCHANNEL> music;
	std::list<MusicId> musicHoles;
	std::map<std::string, AudioId> soundCache;


	static const int DEVICE;
	static const int FRECUENCY;
};

#endif // !_MODULEAUDIO_H_

