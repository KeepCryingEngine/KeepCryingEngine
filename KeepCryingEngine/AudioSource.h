#ifndef _AUDIOSOURCE_H_
#define _AUDIOSOURCE_H_

#include "Component.h"
#include "ModuleAudio.h"

enum class SourceStates
{
	PLAYING,
	WAITING_TO_PLAY,
	WAITING_TO_STOP,
	WAITING_TO_PAUSE,
	WAITING_TO_UNPAUSE,
	STOPPED,
	PAUSED
};

class AudioSource: public Component
{
public:
	static const Component::Type TYPE = Component::Type::AudioSource;
	AudioSource();
	~AudioSource();

	void Awake() override;
	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;

	void UpdateChannelForAudio();

	void DrawUI() override;

	void OnStopButtonPressed();

	void OnPauseButtonPressed();

	void OnPlayButtonPressed();

	void SetMusic(AudioClip* audioInfo);
	void SetVolume(float value);
	void SetPitch(float value);
	void SetPan(float value);
	void SetMaxDistance(float value);
	void SetRollOffFactor(float value);
	void SetDoplerFactor(float value);
	void SetLoop(bool value);

	AudioClip* GetMusic() const;
	float GetVolume() const;
	float GetPitch() const;
	float GetPan() const;
	float GetMaxDistance() const;
	float GetRollOffFactor() const;
	float GetDoplerFactor() const;
	bool GetLoop();

private:
	void OnLoadButtonPressed(const std::experimental::filesystem::path& path);

public:

	DWORD id = 0;

private:
	SourceStates state = SourceStates::STOPPED;
	AudioClip* audioClip = nullptr;
	

	float volume = 1;
	float pitch = 100;
	float pan = 0;
	float minDistance = 0;
	float maxDistance = 10;
	float rollOffFactor = 1;
	float doplerFactor = 1;
	bool loop = false;

	// DWORD id = 0;

	ChannelType loadingChannelType = ChannelType::Mono;
	AudioType loadingAudioType = AudioType::SFX;
};

#endif // !_AUDIOSOURCE_H_

