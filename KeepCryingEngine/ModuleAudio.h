#ifndef _MODULEAUDIO_H_
#define _MODULEAUDIO_H_

#include <string>

#include "Module.h"

typedef unsigned MusicId;

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio();

	bool Init() override;
	bool CleanUp() override;
	update_status PostUpdate(float deltaTimeS, float realDeltaTimeS) override;

	MusicId Load(const std::string& path, const std::string& name);
};

#endif // !_MODULEAUDIO_H_

