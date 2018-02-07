#ifndef _MODULESHADER_H_
#define _MODULESHADER_H_

#include <GL/glew.h>
#include <list>
#include <initializer_list>

#include "Module.h"

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
private:
	void SetUpCameraProgram();

public:
	uint cameraProgramId = 0;
};

#endif