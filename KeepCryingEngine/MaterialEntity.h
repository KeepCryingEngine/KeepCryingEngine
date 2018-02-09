#ifndef _MATERIALENTITY_H_
#define _MATERIALENTITY_H_

#include <GL/glew.h>
#include "ModuleShader.h"

class MaterialEntity
{
public:
	MaterialEntity();
	virtual ~MaterialEntity();

	void DrawUI();

	GLuint GetProgramId() const;
	GLuint GetTextureId() const;

private:
	void SetTexture(const char* path);

private:
	ShaderType shaderType;
	GLuint programId = 0;

	GLuint textureId = 0;
};

#endif // !_MATERIALENTITY_H_