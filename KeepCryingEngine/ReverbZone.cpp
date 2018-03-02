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

	env = EAX_ENVIRONMENT_HANGAR;
	decay = 3;
	damp = 0;
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
		StoreEAXConfig();
		audioListener = activeAudioListener;
	}
	else if(audioListener != nullptr && !newActiveAudioListenerDetected)
	{
		audioListener = nullptr;
	}

	if(audioListener != nullptr)
	{
		ApplyEAXConfig(volume);
	}
	else
	{
		ApplyStoredEAXConfig();
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

void ReverbZone::StoreEAXConfig()
{
	BASS_GetEAXParameters(&storedEnv, NULL, &storedDecay, &storedDamp);
}

void ReverbZone::ApplyStoredEAXConfig()
{
	BASS_SetEAXParameters(storedEnv, -1, storedDecay, storedDamp);
}

void ReverbZone::ApplyEAXConfig(float vol)
{
	BASS_SetEAXParameters(env, vol, decay, damp);
}