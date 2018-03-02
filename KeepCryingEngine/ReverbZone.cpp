#include "ReverbZone.h"

#include "Transform.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "AudioListener.h"

using namespace std;

ReverbZone::ReverbZone() :
	Component(ReverbZone::TYPE)
{
	minDistanceSphere.SetNegativeInfinity();
	maxDistanceSphere.SetNegativeInfinity();
}

ReverbZone::~ReverbZone()
{ }

void ReverbZone::Start()
{
	// TEST

	SetMinDistance(5.0f);
	SetMaxDistance(10.0f);

	reverbConfig.fHighFreqRTRatio = 0.5f;
	reverbConfig.fInGain = -20.0f;
	reverbConfig.fReverbMix = -30.0f;
	reverbConfig.fReverbTime = 1500.0f;

	/* env = EAX_ENVIRONMENT_HANGAR;
	decay = 3;
	damp = 0; */
}

void ReverbZone::Update(float deltaTimeS, float realDeltaTimeS)
{
	SetMinDistance(minDistance);
	SetMaxDistance(maxDistance);

	CheckAudioListener();
}

void ReverbZone::DrawUI()
{
	if(ImGui::CollapsingHeader("Reverb Zone"))
	{

	}
}

vector<Component::Type> ReverbZone::GetNeededComponents() const
{
	return { Transform::TYPE };
}

vector<Component::Type> ReverbZone::GetProhibitedComponents() const
{
	return { ReverbZone::TYPE };
}

void ReverbZone::SetMinDistance(float minDistance)
{
	SetDistanceAndComputeSphere(this->minDistance, minDistanceSphere, minDistance);
}

void ReverbZone::SetMaxDistance(float maxDistance)
{
	SetDistanceAndComputeSphere(this->maxDistance, maxDistanceSphere, maxDistance);
}

void ReverbZone::SetDistanceAndComputeSphere(float& distance, Sphere& sphere, float distanceValue)
{
	const float3& center = gameObject->GetTransform()->GetWorldPosition();

	distance = distanceValue;
	sphere = Sphere(center, distance);
}

void ReverbZone::CheckAudioListener()
{
	const AudioListener* activeAudioListener = App->audio->GetActiveListener();

	float volume = CheckAudioListenerCollision(activeAudioListener);
	bool newActiveAudioListenerDetected = volume > 0.0f;

	if(audioListener == nullptr && newActiveAudioListenerDetected)
	{
		StoreReverbConfig();
		audioListener = activeAudioListener;
	}
	else if(audioListener != nullptr && !newActiveAudioListenerDetected)
	{
		audioListener = nullptr;
		ApplyReverbConfig(true);
	}

	if(newActiveAudioListenerDetected)
	{
		ApplyReverbConfig();
	}
}

float ReverbZone::CheckAudioListenerCollision(const AudioListener* audioListener) const
{
	float volume = -1.0f;

	if(audioListener != nullptr)
	{
		const float3& audioListenerPosition = audioListener->gameObject->GetTransform()->GetWorldPosition();

		if(maxDistanceSphere.Contains(audioListenerPosition))
		{
			float distanceBetweenSpheres = maxDistance - minDistance; // minDistanceSphere.Distance(maxDistanceSphere);
			float minDistanceSphereDistance = minDistanceSphere.Centroid().Distance(audioListenerPosition) - minDistance;

			volume = 1.0f - minDistanceSphereDistance / distanceBetweenSpheres;

			if(volume > 1.0f)
			{
				volume = 1.0f;
			}
		}
	}

	return volume;
}

void ReverbZone::StoreReverbConfig()
{
	BASS_FXGetParameters(BASS_FX_DX8_REVERB, &storedReverbConfig);

	// BASS_GetEAXParameters(&storedEnv, NULL, &storedDecay, &storedDamp);
}

void ReverbZone::ApplyReverbConfig(bool stored)
{
	if(stored)
	{
		LOG_DEBUG("STORED: %f, %f, %f, %f", storedReverbConfig.fHighFreqRTRatio, storedReverbConfig.fInGain, storedReverbConfig.fReverbMix, storedReverbConfig.fReverbTime);

		BASS_FXSetParameters(BASS_FX_DX8_REVERB, &storedReverbConfig);
	}
	else
	{
		LOG_DEBUG("REVERB: %f, %f, %f, %f", reverbConfig.fHighFreqRTRatio, reverbConfig.fInGain, reverbConfig.fReverbMix, reverbConfig.fReverbTime);

		BASS_FXSetParameters(BASS_FX_DX8_REVERB, &reverbConfig);
	}

	// BASS_SetEAXParameters(storedEnv, -1, storedDecay, storedDamp);
}

/* void ReverbZone::ApplyEAXConfig()
{
	BASS_FXSetParameters(BASS_FX_DX8_REVERB, &reverbConfig);

	// BASS_SetEAXParameters(env, vol, decay, damp);
} */