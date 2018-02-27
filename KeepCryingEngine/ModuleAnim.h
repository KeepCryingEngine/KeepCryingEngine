#ifndef _MODULEANIM_H_
#define _MODULEANIM_H_

#include <assimp\anim.h>
#include <map>
#include <vector>

#include "Module.h"

struct NodeAnim
{
	aiString name;
	aiVector3D* positions = nullptr;
	aiQuaternion* rotations = nullptr;
	unsigned num_positions = 0;
	unsigned num_rotations = 0;
};

struct Anim
{
	unsigned duration = 0;
	unsigned num_chanels = 0;
	NodeAnim* chanels = nullptr;
};

struct AnimInstance
{
	Anim* anim;
	unsigned time = 0;
	bool loop = true;
	AnimInstance* next = nullptr;
	unsigned blend_duration = 0;
	unsigned blend_time = 0;
};

typedef int AnimInstanceId;
typedef std::map<aiString, Anim*> AnimMap;
typedef std::vector<AnimInstance*> InstanceList;
typedef std::vector<AnimInstanceId> HoleList;


class ModuleAnim : public Module
{
public:
	ModuleAnim();
	~ModuleAnim();

	bool CleanUp() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

	void Load(const char* name, const char* file);
	AnimInstanceId Play(const char* name);
	void Stop(AnimInstanceId id);
	void BlendTo(AnimInstanceId id, const char* name, unsigned blend_time);

	bool GetTransform(AnimInstanceId id, const char* channel, aiVector3D& position, aiQuaternion& rotation)const;



private:
	AnimMap animations;
	InstanceList instances;
	HoleList holes;
};


#endif // !_MODULEANIM_H_

