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
	Anim* anim;
	unsigned time = 0;
	bool loop = true;
	AnimInstance* next = nullptr;
	unsigned blend_duration = 0;
	unsigned blend_time = 0;
};

typedef unsigned AnimInstanceId;
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

	void Load(const std::string& path, const std::string& name);
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

