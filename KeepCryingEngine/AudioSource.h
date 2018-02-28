#ifndef _AUDIOSOURCE_H_
#define _AUDIOSOURCE_H_

#include "Component.h"
#include "ModuleAudio.h"

enum class AudioMode
{
	STEREO,
	MONO
};

class AudioSource: public Component
{
public:
	AudioSource();
	~AudioSource();

	void Awake() override;
	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;

	void DrawUI() override;

	void SetMusic(MusicId id);
	void SetMode(AudioMode newMode);
	void SetPitch(float value);
	void SetPan(float value);
	void SetMaxDistance(float value);
	void SetRollOffFactor(float value);
	void SetDoplerFactor(float value);

	MusicId GetMusic() const;
	AudioMode GetMode() const;
	float GetPitch() const;
	float GetPan() const;
	float GetMaxDistance() const;
	float GetRollOffFactor() const;
	float GetDoplerFactor() const;
private:
	MusicId id;
	AudioMode mode;
	float pitch;
	float pan;
	float maxDistance;
	float rollOffFactor;
	float doplerFactor;

};

#endif // !_AUDIOSOURCE_H_

