#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <GL/glew.h>
#include "ModuleShader.h"

class Texture;

class Material
{
public:
	Material();
	Material(const Material& mat);
	virtual ~Material();

	void DrawUI();

	GLuint GetProgramId() const;
	Texture* GetTexture() const;
	void SetTexture(const char* path);

private:
	ShaderType shaderType = ShaderType::Default;
	GLuint programId = 0;
	Texture* texture = nullptr;
};

#endif // !_MATERIALENTITY_H_