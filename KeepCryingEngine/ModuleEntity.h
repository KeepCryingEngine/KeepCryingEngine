#ifndef _MODULEENTITY_H_
#define _MODULEENTITY_H_

#include "Module.h"

class MeshEntity;

class ModuleEntity : public Module
{
public:
	ModuleEntity();
	~ModuleEntity();

	bool Init() override;
	bool CleanUp() override;

	MeshEntity* GetCube();
	MeshEntity* GetSphere();

private:
	void SetUpCube();
	void SetUpSphere();

private:
	MeshEntity* cube;
	MeshEntity* sphere;
};

#endif // !_MODULEENTITY_H_
