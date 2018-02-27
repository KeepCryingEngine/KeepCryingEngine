#include "ModuleAnim.h"

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

using namespace  std;

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
	return update_status::UPDATE_CONTINUE;
}

void ModuleAnim::Load(const string& path, const string& name)
{
	const aiScene* scene = aiImportFile((path + name).c_str(), aiProcess_Triangulate);
	for(int i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* currentAnim = scene->mAnimations[i];
		Anim* newAnim = new Anim();
		newAnim->duration = currentAnim->mDuration;
		newAnim->numChanels = currentAnim->mNumChannels;
		NodeAnim* tempNodeAnimPointer = new NodeAnim[currentAnim->mNumChannels];
		for(int j = 0; j < currentAnim->mNumChannels; j++)
		{
			aiNodeAnim* currentChanel = currentAnim->mChannels[j];
			NodeAnim* newChanel = &tempNodeAnimPointer[j];
			newChanel->name = currentChanel->mNodeName;
			newChanel->numPositions = currentChanel->mNumPositionKeys;
			newChanel->numRotations = currentChanel->mNumRotationKeys;
			aiVector3D* tempPosPointer = new aiVector3D[currentChanel->mNumPositionKeys];
			for(int k = 0; k < currentChanel->mNumPositionKeys; k++)
			{
				tempPosPointer[k] = currentChanel->mPositionKeys[k].mValue;
			}
			newChanel->positions = tempPosPointer;
			aiQuaternion* tempQuaternionPointer = new aiQuaternion[currentChanel->mNumRotationKeys];
			for(int k = 0; k < currentChanel->mNumRotationKeys; k++)
			{
				tempQuaternionPointer[k] = currentChanel->mRotationKeys[k].mValue;
			}
			newChanel->rotations = tempQuaternionPointer;
		}
		newAnim->chanels = tempNodeAnimPointer;
	}
}

AnimInstanceId ModuleAnim::Play(const char * name)
{
	return AnimInstanceId();
}

void ModuleAnim::Stop(AnimInstanceId id)
{}

void ModuleAnim::BlendTo(AnimInstanceId id, const char * name, unsigned blend_time)
{}

bool ModuleAnim::GetTransform(AnimInstanceId id, const char * channel, aiVector3D & position, aiQuaternion & rotation) const
{
	return false;
}
