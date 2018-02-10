#ifndef _MODULESHADER_H_
#define _MODULESHADER_H_

#include <GL/glew.h>
#include <list>
#include <initializer_list>

#include "Module.h"

enum class ShaderType {
	Default,
	Cartoon,
	Color,
	Depth
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

private:

	void SetUpColorProgram();
	void SetUpDefaultShader();
	void SetUpCartoonShader();
	void SetUpDepthShader();

private:
	GLuint cartoonShaderId = 0;
	GLuint defaultShaderId = 0;
	GLuint colorShaderId = 0;
	GLuint depthShaderId = 0;
};

#endif