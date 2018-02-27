#include "ModuleAnim.h"

ModuleAnim::ModuleAnim()
{}

ModuleAnim::~ModuleAnim()
{}

bool ModuleAnim::CleanUp()
{
	return false;
}

update_status ModuleAnim::Update(float deltaTimeS, float realDeltaTimeS)
{
	return update_status();
}

void ModuleAnim::Load(const char * name, const char * file)
{}

AnimInstanceId ModuleAnim::Play(const char * name)
{
	return AnimInstanceId();
}

void ModuleAnim::Stop(AnimInstanceId id)
{}

void ModuleAnim::BlendTo(AnimInstanceId id, const char * name, unsigned blend_time)
{}

bool ModuleAnim::GetTransform(AnimInstanceId id, const char * channel, aiVector3D & position, aiQuaternion & rotation) const
{
	return false;
}
