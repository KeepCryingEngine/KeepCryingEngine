#ifndef _SOUNDEFFECTS_H_
#define _SOUNDEFFECTS_H_

#include <bass.h>
#include <list>

struct EffectInfo
{
	BASS_DX8_I3DL2REVERB* reverbConfig;
	int priority;
};

class SoundEffects
{
public:
	SoundEffects();
	~SoundEffects();

	void AddEffect(EffectInfo& effect);
	void RemoveEffect(EffectInfo& effect);

	const std::list<EffectInfo*>& GetEffects();
private:
	void Callback();

private:
	std::list<EffectInfo*> effectsList;
};

#endif // !_SOUNDEFFECTS_H_

