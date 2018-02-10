#ifndef _MODULESHADER_H_
#define _MODULESHADER_H_

#include <GL/glew.h>
#include <list>
#include <initializer_list>

#include "Module.h"

enum class ShaderType {
	Default,
	Cartoon
};

class ModuleShader : public Module
{
public:
	ModuleShader();
	virtual ~ModuleShader();

	bool Init() override;

	uint AddShaderPath(const char* path, GLenum shaderType);
	uint AddShader(const char* source, GLenum shaderType);
	
	uint AddProgram(const std::list<uint>& shaders);
	uint AddProgram(std::initializer_list<uint> shaders);

	GLuint GetShaderId(ShaderType shaderType) const;

public:
	uint cameraProgramId = 0;

private:
	void SetUpCameraProgram();

	void SetUpDefaultShader();
	void SetUpCartoonShader();

private:
	GLuint cartoonShaderId = 0;
	GLuint defaultShaderId = 0;
};

#endif