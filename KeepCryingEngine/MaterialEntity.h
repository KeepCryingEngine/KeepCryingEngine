#ifndef _MATERIALENTITY_H_
#define _MATERIALENTITY_H_

#include <GL/glew.h>

class MaterialEntity
{
public:
	MaterialEntity();
	virtual ~MaterialEntity();

	GLuint GetProgramId() const;
	GLuint GetTextureId() const;

private:
	GLuint programId = 0;
	GLuint textureId = 0;
};

#endif // !_MATERIALENTITY_H_