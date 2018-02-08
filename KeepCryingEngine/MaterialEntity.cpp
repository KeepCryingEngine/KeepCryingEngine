#include "MaterialEntity.h"



MaterialEntity::MaterialEntity()
{
}


MaterialEntity::~MaterialEntity()
{
}

GLuint MaterialEntity::GetProgramId() const
{
	return programId;
}

GLuint MaterialEntity::GetTextureId() const
{
	return textureId;
}
