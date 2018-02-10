#include "ModuleEntity.h"

#include "MeshEntity.h"
#include "Globals.h"

ModuleEntity::ModuleEntity()
{}

ModuleEntity::~ModuleEntity()
{}

bool ModuleEntity::Init()
{
	SetUpCube();
	SetUpSphere();
	return true;
}

bool ModuleEntity::CleanUp()
{
	RELEASE(cube);
	RELEASE(sphere);
	return true;
}

MeshEntity * ModuleEntity::GetCube()
{
	return cube;
}

MeshEntity * ModuleEntity::GetSphere()
{
	return sphere;
}

void ModuleEntity::SetUpCube()
{
	cube = new MeshEntity();
	cube->SetUpCube();
}

void ModuleEntity::SetUpSphere()
{
	sphere = new MeshEntity();
	sphere->SetUpSphere();
}
