#include "ModuleAnim.h"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

using namespace std;

ModuleAnim::ModuleAnim()
{}

ModuleAnim::~ModuleAnim()
{}

bool ModuleAnim::CleanUp()
{
	holes.clear();
	instances.clear();
	animations.clear();

	return true;
}

update_status ModuleAnim::Update(float deltaTimeS, float realDeltaTimeS)
{
	unsigned time = (uint)(1000 * deltaTimeS);

	for(size_t i = 0; i < instances.size(); ++i)
	{
		AnimInstance*& animInstance = instances[i];

		if(animInstance == nullptr)
		{
			continue;
		}

		animInstance->time += time;

		Anim* anim = animInstance->anim;

		if(anim)
		{
			if(animInstance->time >= anim->duration)
			{
				if(animInstance->loop)
				{
					animInstance->time %= anim->duration;
				}
				else
				{
					animInstance->time = anim->duration;
				}
			}
		}

		// Blend

		AnimInstance* nextAnimInstance = animInstance->next;

		if(nextAnimInstance)
		{
			animInstance->blend_time += time;

			if(animInstance->blend_time >= animInstance->blend_duration)
			{
				// Blend end

				RELEASE(animInstance);
				instances[i] = nextAnimInstance;
			}
		}
	}

	return update_status::UPDATE_CONTINUE;
}

set<string> ModuleAnim::Load(const std::experimental::filesystem::path& path)
{
	set<string> animationNames;

	const aiScene* scene = aiImportFile(path.string().c_str(), aiProcess_Triangulate);

	if(scene != nullptr)
	{
		for(uint i = 0; i < scene->mNumAnimations; i++)
		{
			aiAnimation* currentAnim = scene->mAnimations[i];

			animationNames.insert(currentAnim->mName.C_Str());

			Anim* newAnim = new Anim();
			newAnim->duration = (uint)(1000 * currentAnim->mDuration / currentAnim->mTicksPerSecond);
			newAnim->numChanels = currentAnim->mNumChannels;
			NodeAnim* tempNodeAnimPointer = new NodeAnim[currentAnim->mNumChannels];
			for(uint j = 0; j < currentAnim->mNumChannels; j++)
			{
				aiNodeAnim* currentChanel = currentAnim->mChannels[j];
				NodeAnim* newChanel = &tempNodeAnimPointer[j];
				newChanel->name = currentChanel->mNodeName;
				newChanel->numPositions = currentChanel->mNumPositionKeys;
				newChanel->numRotations = currentChanel->mNumRotationKeys;
				aiVector3D* tempPosPointer = new aiVector3D[currentChanel->mNumPositionKeys];
				for(uint k = 0; k < currentChanel->mNumPositionKeys; k++)
				{
					tempPosPointer[k] = currentChanel->mPositionKeys[k].mValue;
				}
				newChanel->positions = tempPosPointer;
				aiQuaternion* tempQuaternionPointer = new aiQuaternion[currentChanel->mNumRotationKeys];
				for(uint k = 0; k < currentChanel->mNumRotationKeys; k++)
				{
					tempQuaternionPointer[k] = currentChanel->mRotationKeys[k].mValue;
				}
				newChanel->rotations = tempQuaternionPointer;
			}
			newAnim->chanels = tempNodeAnimPointer;

			animations[currentAnim->mName.C_Str()] = newAnim;
		}
	}

	return animationNames;
}

AnimInstanceId ModuleAnim::Play(const char* name)
{
	AnimInstanceId animInstanceId;

	if(!holes.empty())
	{
		animInstanceId = holes.front();
		holes.pop_front();
	}
	else
	{
		animInstanceId = instances.size();
	}

	Anim* anim = nullptr;

	AnimMap::iterator animationIt = animations.find(name);

	if(animationIt != animations.end())
	{
		anim = animationIt->second;
	}

	AnimInstance* animInstance = new AnimInstance();

	animInstance->anim = anim;
	// ...

	if(animInstanceId < instances.size())
	{
		instances[animInstanceId] = animInstance;
	}
	else
	{
		instances.push_back(animInstance);
	}

	return animInstanceId;
}

void ModuleAnim::Stop(AnimInstanceId id)
{
	if(instances[id] != nullptr)
	{
		holes.push_back(id);
		RELEASE(instances[id]);
	}
}

void ModuleAnim::BlendTo(AnimInstanceId id, const char * name, unsigned blend_time)
{
	AnimInstance* animInstance = instances[id];

	if(animInstance == nullptr)
	{
		return;
	}

	animInstance = FindNextBlendingAnimInstance(animInstance);

	animInstance->next = new AnimInstance();

	animInstance->blend_duration = blend_time;

	Anim* nextAnim = nullptr;

	AnimMap::iterator animationIt = animations.find(name);

	if(animationIt != animations.end())
	{
		nextAnim = animationIt->second;
	}

	animInstance->next->anim = nextAnim;
	// ...
}

bool ModuleAnim::GetTransform(AnimInstanceId id, const char * channel, aiVector3D & position, aiQuaternion & rotation) const
{
	if(instances[id] != nullptr)
	{
		return GetTransform(instances[id], channel, position, rotation);
	}

	return false;
}

float ModuleAnim::GetPercent(AnimInstanceId id) const
{
	float percent = 0.0f;

	AnimInstance* animInstance = instances[id];

	if(animInstance)
	{
		Anim* anim = animInstance->anim;

		if(anim)
		{
			uint currentTime = animInstance->time;
			uint totalTime = anim->duration;

			percent = (float)currentTime / (float)totalTime;
		}
	}

	return percent;
}

bool ModuleAnim::GetTransform(AnimInstance* animInstance, const char * channel, aiVector3D & position, aiQuaternion & rotation) const
{
	if(animInstance == nullptr)
	{
		return false;
	}

	Anim* anim = animInstance->anim;

	if(anim == nullptr)
	{
		return false;
	}

	NodeAnim* node = nullptr;

	for(size_t i = 0; i < anim->numChanels; ++i)
	{
		NodeAnim* nodeAnim = &anim->chanels[i];

		if(strcmp(nodeAnim->name.C_Str(), channel) == 0)
		{
			node = nodeAnim;
			break;
		}
	}

	float posKey = float(animInstance->time * (node->numPositions - 1)) / float(anim->duration);
	float rotKey = float(animInstance->time * (node->numRotations - 1)) / float(anim->duration);

	uint posIndex = uint(posKey);
	uint rotIndex = uint(rotKey);

	float posLambda = posKey - float(posIndex);
	float rotLambda = rotKey - float(rotIndex);

	position = node->positions[posIndex];
	rotation = node->rotations[rotIndex];

	uint posIndexNext = (posIndex + 1) % node->numPositions;
	uint rotIndexNext = (rotIndex + 1) % node->numRotations;

	const aiVector3D& positionNext = node->positions[posIndexNext];
	const aiQuaternion& rotationNext = node->rotations[rotIndexNext];

	position = Lerp(position, positionNext, posLambda);
	rotation = Lerp(rotation, rotationNext, rotLambda);

	// Blend

	AnimInstance* nextAnimInstance = animInstance->next;

	if(nextAnimInstance)
	{
		float blendLambda = float(animInstance->blend_time) / float(animInstance->blend_duration);

		aiVector3D positionNextAnim;
		aiQuaternion rotationNextAnim;

		GetTransform(nextAnimInstance, channel, positionNextAnim, rotationNextAnim);

		position = Lerp(position, positionNextAnim, blendLambda);
		rotation = Lerp(rotation, rotationNextAnim, blendLambda);
	}

	return true;
}

AnimInstance* ModuleAnim::FindNextBlendingAnimInstance(AnimInstance * animInstance) const
{
	if(animInstance != nullptr && animInstance->next != nullptr)
	{
		return FindNextBlendingAnimInstance(animInstance->next);
	}

	return animInstance;
}

aiVector3D ModuleAnim::Lerp(const aiVector3D & first, const aiVector3D & second, float lambda) const
{
	return (1.0f - lambda) * first + lambda * second;
}

aiQuaternion ModuleAnim::Lerp(const aiQuaternion & first, const aiQuaternion & second, float lambda) const
{
	/*

	aiQuaternion result;

	aiQuaternion::Interpolate(result, first, second, lambda);

	result.Normalize();

	return result;

	*/

	aiQuaternion result;

	float dot = first.x * second.x + first.y * second.y + first.z * second.z + first.w * second.w;

	if(dot < 0.0f)
	{
		lambda *= -1.0f;
	}

	result.x = (1.0f - lambda) * first.x + lambda * second.x;
	result.y = (1.0f - lambda) * first.y + lambda * second.y;
	result.z = (1.0f - lambda) * first.z + lambda * second.z;
	result.w = (1.0f - lambda) * first.w + lambda * second.w;

	result.Normalize();

	return result;
}