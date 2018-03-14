#ifndef _MODULEANIM_H_
#define _MODULEANIM_H_

#include <assimp\anim.h>
#include <set>
#include <map>
#include <list>
#include <vector>
#include <experimental/filesystem>

#include "Module.h"

struct NodeAnim
{
	aiString name;
	aiVector3D* positions = nullptr;
	aiQuaternion* rotations = nullptr;
	unsigned numPositions = 0;
	unsigned numRotations = 0;
};

struct Anim
{
	unsigned duration = 0;
	unsigned numChanels = 0;
	NodeAnim* chanels = nullptr;
};

struct AnimInstance
{
	Anim* anim = nullptr;
	unsigned time = 0;
	bool loop = true;
	AnimInstance* next = nullptr;
	unsigned blend_duration = 0;
	unsigned blend_time = 0;
};

typedef unsigned AnimInstanceId;
typedef std::map<std::string, Anim*> AnimMap;
typedef std::vector<AnimInstance*> InstanceList;
typedef std::list<AnimInstanceId> HoleList;

class ModuleAnim : public Module
{
public:
	ModuleAnim();
	virtual ~ModuleAnim();

	bool CleanUp() override;
	update_status Update() override;

	std::set<std::string> Load(const std::experimental::filesystem::path& path);
	AnimInstanceId Play(const char* name);
	void Stop(AnimInstanceId id);
	void BlendTo(AnimInstanceId id, const char* name, unsigned blend_time);

	bool GetTransform(AnimInstanceId id, const char* channel, aiVector3D& position, aiQuaternion& rotation) const;

	float GetPercent(AnimInstanceId id) const;

private:
	bool GetTransform(AnimInstance* animInstance, const char* channel, aiVector3D& position, aiQuaternion& rotation) const;

	AnimInstance* FindNextBlendingAnimInstance(AnimInstance* animInstance) const;

	aiVector3D Lerp(const aiVector3D& first, const aiVector3D& second, float lambda) const;

	aiQuaternion Lerp(const aiQuaternion& first, const aiQuaternion& second, float lambda) const;

private:
	AnimMap animations;
	InstanceList instances;
	HoleList holes;
};

#endif // !_MODULEANIM_H_