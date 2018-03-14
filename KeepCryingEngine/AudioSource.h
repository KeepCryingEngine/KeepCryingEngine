#ifndef _AUDIOSOURCE_H_
#define _AUDIOSOURCE_H_

#include "Component.h"
#include "ModuleAudio.h"
#include "SoundsEffects.h"

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

	void DrawUI() override;

	void UpdateChannelEffects();

	void SetMusic(AudioClip* audioInfo);
	void SetVolume(float value);
	void SetPitch(float value);
	//void SetPan(float value);
	void SetMaxDistance(float value);
	void SetRollOffFactor(float value);
	void SetDoplerFactor(float value);
	void SetLoop(bool value);

	AudioClip* GetMusic() const;
	float GetVolume() const;
	float GetPitch() const;
	//float GetPan() const;
	float GetMaxDistance() const;
	float GetRollOffFactor() const;
	float GetDoplerFactor() const;
	bool GetLoop();

	virtual void Load(const nlohmann::json& json) override;
	virtual void Save(nlohmann::json& json) const override;

private:
	void OnLoadButtonPressed(const std::experimental::filesystem::path& path);
	void OnStopButtonPressed();
	void OnPauseButtonPressed();
	void OnPlayButtonPressed();

	DWORD GetChannelForAudio(const AudioClip* audioClip) const;

	void ClearChannelEffects();

public:
	DWORD channel = 0;

private:
	SourceStates state = SourceStates::STOPPED;
	AudioClip* audioClip = nullptr;
	
	float volume = 1;
	float originalFreq = 0;
	float freqModifier = 0;
	float minDistance = 0;
	float maxDistance = 10;
	float rollOffFactor = 1;
	float doplerFactor = 1;
	bool loop = false;

	std::experimental::filesystem::path path;

	ChannelType usedChannelType;
	AudioType usedAudioType;

	// DWORD id = 0;

	ChannelType loadingChannelType = ChannelType::Mono;
	AudioType loadingAudioType = AudioType::SFX;

	std::list<HFX>activeEffects;

	//BASS_BFX_PITCHSHIFT pitchConfig;
};

#endif // !_AUDIOSOURCE_H_