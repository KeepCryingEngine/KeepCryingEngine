#include "ReverbZone.h"

#include <LineSegment.h>

#include "Transform.h"
#include "GameObject.h"
#include "Application.h"
#include "AudioSource.h"
#include "ModuleRender.h"
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

	SetMinDistance(minDistance);
	SetMaxDistance(maxDistance);

	// SetUp reverbConfig
}

void ReverbZone::RealUpdate(float deltaTimeS, float realDeltaTimeS)
{
	SetMinDistance(minDistance);
	SetMaxDistance(maxDistance);

	CheckAudioListener();

	if(mode == 0)
	{
		DrawCube(minDistanceCube, float3(255.0f, 0.0f, 0.0f));
		DrawCube(maxDistanceCube, float3(0.0f, 0.0f, 255.0f));
	}
	else if(mode == 1)
	{
		DrawSphere(minDistanceSphere, float3(255.0f, 0.0f, 0.0f));
		DrawSphere(maxDistanceSphere, float3(0.0f, 0.0f, 255.0f));
	}
}

void ReverbZone::Destroy()
{
	DeapplyReverbConfig();
}

void ReverbZone::DrawUI()
{
	if(ImGui::CollapsingHeader("Reverb Zone"))
	{
		if(ImGui::Checkbox("Active", &enabled))
		{
			if(!enabled)
			{
				DeapplyReverbConfig();
			}
		}

		ImGui::SameLine();

		if(ImGui::Button("Delete Component"))
		{
			gameObject->RemoveComponent(this);
		}

		static float minDistanceTmp = minDistance;

		if(ImGui::DragFloat("Min distance", &minDistanceTmp, 0.05f, 0.001f, 100.0f))
		{
			SetMinDistance(minDistanceTmp);
			minDistanceTmp = minDistance;
		}

		static float maxDistanceTmp = maxDistance;

		if(ImGui::DragFloat("Max distance", &maxDistanceTmp, 0.05f, 0.001f, 100.0f))
		{
			SetMaxDistance(maxDistanceTmp);
			maxDistanceTmp = maxDistance;
		}

		if(ImGui::Combo("Mode", &mode, "Cube\0Sphere"))
		{
			SetMode(mode);
		}

		ImGui::Indent(16.0f);

		if(ImGui::CollapsingHeader("Config"))
		{
			ImGui::DragInt("Room Atten.", &reverbConfig.lRoom, 10,-10000, 0);
			ImGui::DragInt("Room Atten. (HF)", &reverbConfig.lRoomHF, 10, -10000, 0);
			ImGui::DragFloat("RollOff Factor", &reverbConfig.flRoomRolloffFactor, 0.1f, 0.0f, 10.0f);
			ImGui::DragFloat("Decay Time", &reverbConfig.flDecayTime, 0.1f, 0.1f, 20.0f);
			ImGui::DragFloat("Decay Ratio (HF)", &reverbConfig.flDecayHFRatio, 0.1f, 0.1f, 2.0f);
			ImGui::DragInt("Reflection Atten.", &reverbConfig.lReflections, 10, -10000, 1000);
			ImGui::DragFloat("Reflection Delay", &reverbConfig.flReflectionsDelay, 0.01f, 0.0f, 0.3f);
			ImGui::DragInt("Reverb Atten.", &reverbConfig.lReverb, 100, -10000, 2000);
			ImGui::DragFloat("Reverb Delay", &reverbConfig.flReverbDelay, 0.01f, 0.0f, 0.1f);
			ImGui::DragFloat("Echo Density", &reverbConfig.flDiffusion, 1.0f, 0.0f, 100.0f);
			ImGui::DragFloat("Modal Density", &reverbConfig.flDensity, 1.0f, 0.0f, 100.0f);
			ImGui::DragFloat("Reference (HF)", &reverbConfig.flHFReference, 10.0f, 20.0f, 20000.0f);
		}
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
	if(minDistance < maxDistance)
	{
		SetDistanceAndComputeCube(this->minDistance, minDistanceCube, minDistance);
		SetDistanceAndComputeSphere(this->minDistance, minDistanceSphere, minDistance);
	}
}

void ReverbZone::SetMaxDistance(float maxDistance)
{
	if(maxDistance > minDistance)
	{
		SetDistanceAndComputeCube(this->maxDistance, maxDistanceCube, maxDistance);
		SetDistanceAndComputeSphere(this->maxDistance, maxDistanceSphere, maxDistance);
	}
}

void ReverbZone::SetMode(int mode)
{
	this->mode = mode;
	DeapplyReverbConfig();
}

void ReverbZone::SetDistanceAndComputeCube(float& distance, AABB& cube, float distanceValue)
{
	const float3& center = gameObject->GetTransform()->GetWorldPosition();

	distance = distanceValue;
	cube = AABB(Sphere(center, distance));
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
		ApplyReverbConfig();
		audioListener = activeAudioListener;
	}
	else if(audioListener != nullptr && !newActiveAudioListenerDetected)
	{
		DeapplyReverbConfig();
	}
}

float ReverbZone::CheckAudioListenerCollision(const AudioListener* audioListener) const
{
	float volume = -1.0f;

	if(audioListener != nullptr)
	{
		const float3& audioListenerPosition = audioListener->gameObject->GetTransform()->GetWorldPosition();

		bool contained = false;
		float computedMinDistance = 0.0f;

		if(mode == 0)
		{
			contained = maxDistanceCube.Contains(audioListenerPosition);
			computedMinDistance = minDistanceCube.Centroid().Distance(audioListenerPosition) - sqrtf(2.0f) * minDistance;
		}
		else if(mode == 1)
		{
			contained = maxDistanceSphere.Contains(audioListenerPosition);
			computedMinDistance = minDistanceSphere.Centroid().Distance(audioListenerPosition) - minDistance;
		}

		if(contained)
		{
			float distanceBetweenDefinedCollisions = maxDistance - minDistance;

			volume = 1.0f - computedMinDistance / distanceBetweenDefinedCollisions;

			if(volume > 1.0f)
			{
				volume = 1.0f;
			}
		}
	}

	return volume;
}

void ReverbZone::ApplyReverbConfig()
{
	vector<AudioSource*> sources= App->audio->GetAllAudioSources();

	for(AudioSource* audioSource: sources)
	{
		if(audioSource->id != 0)
		{
			hfxValid = true;
			reverbEffect = BASS_ChannelSetFX(audioSource->id, BASS_FX_DX8_I3DL2REVERB, 1);
			BASS_FXSetParameters(reverbEffect, &reverbConfig);
		}
	}
}

void ReverbZone::DeapplyReverbConfig()
{
	if(hfxValid)
	{
		vector<AudioSource*> sources = App->audio->GetAllAudioSources();

		for(AudioSource* audioSource : sources)
		{
			if(audioSource->id != 0)
			{
				hfxValid = false;
				BASS_ChannelRemoveFX(audioSource->id, reverbEffect);
			}
		}
	}

	audioListener = nullptr;
}

void ReverbZone::DrawCube(const AABB& cube, const float3& color) const
{
	App->renderer->DrawCube(cube.Centroid(), color, 0.5f * cube.Edge(0).Length());
}

void ReverbZone::DrawSphere(const Sphere& sphere, const float3& color) const
{
	App->renderer->DrawSphere(sphere.Centroid(), color, sphere.r);
}