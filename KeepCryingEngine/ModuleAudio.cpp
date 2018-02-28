#include "ModuleAudio.h"

using namespace std;

ModuleAudio::ModuleAudio()
{}

ModuleAudio::~ModuleAudio()
{}

bool ModuleAudio::Init()
{
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

MusicId ModuleAudio::Load(const string & path, const string & name)
{
	return MusicId();
}
