#ifndef _REVERBZONE_H_
#define _REVERBZONE_H_

#include <bass.h>
#include <AABB.h>
#include <Sphere.h>

#include "Component.h"
#include "SoundsEffects.h"

class AudioListener;

class ReverbZone : public Component
{
public:
	static const Component::Type TYPE = Component::Type::ReverbZone;

	ReverbZone();
	virtual ~ReverbZone();

	virtual void Awake() override;
	virtual void Start() override;
	virtual void RealUpdate(float deltaTimeS, float realDeltaTimeS) override;
	virtual void Destroy() override;

	void DrawUI() override;

	virtual std::vector<Component::Type> GetNeededComponents() const override;
	virtual std::vector<Component::Type> GetProhibitedComponents() const override;

	void SetMinDistance(float minDistance);
	void SetMaxDistance(float minDistance);

	void SetMode(int mode);

private:
	void SetDistanceAndComputeCube(float& distance, AABB& cube, float distanceValue);
	void SetDistanceAndComputeSphere(float& distance, Sphere& sphere, float distanceValue);

	void CheckAudioListener();
	float CheckAudioListenerCollision(const AudioListener* audioListener) const;

	void ApplyReverbConfig();
	void DeapplyReverbConfig();

	void DrawCube(const AABB& cube, const float3& color) const;
	void DrawSphere(const Sphere& sphere, const float3& color) const;

private:
	int mode = 0; // 0 Cube, 1 Sphere

	AABB minDistanceCube;
	AABB maxDistanceCube;

	Sphere minDistanceSphere;
	Sphere maxDistanceSphere;

	float minDistance = 5.0f;
	float maxDistance = 10.0f;

	const AudioListener* audioListener = nullptr;

	bool hfxValid = false;

	BASS_DX8_I3DL2REVERB reverbConfig;
	EffectInfo* effect;
};

#endif