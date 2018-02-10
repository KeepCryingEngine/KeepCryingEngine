#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <GL/glew.h>
#include "ModuleShader.h"

class Texture;

class Material
{
public:
	Material();
	virtual ~Material();

	void DrawUI();

	GLuint GetProgramId() const;
	Texture* GetTexture() const;

private:
	void SetTexture(const char* path);

private:
	ShaderType shaderType = ShaderType::Default;
	GLuint programId = 0;
	Texture* texture = nullptr;
};

#endif // !_MATERIALENTITY_H_