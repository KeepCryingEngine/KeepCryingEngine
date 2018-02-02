#ifndef _MODULESHADER_H_
#define _MODULESHADER_H_

#include <GL/glew.h>
#include <initializer_list>
#include <list>

#include "Module.h"

class Shader;

class ModuleShader : public Module
{
public:
	ModuleShader();
	virtual ~ModuleShader();

	bool Init() override;

	bool Start() override;

	const Shader* AddShader(const char* source, GLenum shaderType);
	uint AddProgram(std::initializer_list<const Shader*> shaders);
	uint AddProgram(std::list<uint> shaders);

public:
	uint defaultProgramId = 0;
};

#endif