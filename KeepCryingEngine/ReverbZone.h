#ifndef _REVERBZONE_H_
#define _REVERBZONE_H_

#include <bass.h>
#include <Sphere.h>

#include "Component.h"

class AudioListener;

class ReverbZone : public Component
{
public:
	static const Component::Type TYPE = Component::Type::ReverbZone;

	ReverbZone();
	virtual ~ReverbZone();

	virtual void Start() override;

	virtual void Update(float deltaTimeS, float realDeltaTimeS) override;

	void DrawUI() override;

	virtual std::vector<Component::Type> GetNeededComponents() const override;
	virtual std::vector<Component::Type> GetProhibitedComponents() const override;

	void SetMinDistance(float minDistance);
	void SetMaxDistance(float minDistance);

private:
	void SetDistanceAndComputeSphere(float& distance, Sphere& sphere, float distanceValue);

	void CheckAudioListener();
	float CheckAudioListenerCollision(const AudioListener* audioListener) const;

	void StoreReverbConfig();
	void ApplyReverbConfig(bool stored = false);

private:
	Sphere minDistanceSphere;
	Sphere maxDistanceSphere;

	float minDistance = 0.0f;
	float maxDistance = 0.0f;

	const AudioListener* audioListener = nullptr;

	/*

	// EAX config

	unsigned long env = -1;
	float decay = -1, damp = -1;

	unsigned long storedEnv;
	float storedDecay, storedDamp;

	*/

	BASS_DX8_REVERB reverbConfig;

	BASS_DX8_REVERB storedReverbConfig;
};

#endif