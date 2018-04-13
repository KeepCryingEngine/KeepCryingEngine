#ifndef _MODULESHADER_H_
#define _MODULESHADER_H_

#include <GL/glew.h>
#include <list>
#include <map>
#include <initializer_list>

#include "Module.h"

/* enum class ShaderType {
	Default,
	Cartoon,
	Color,
	Depth,
	Diffuse
}; */

enum class ShaderType
{
	Default,
	Lightning,
	Cartoon,
	Depth
};

const int DEFAULT = 0;
const int LIGHTNING = 1 << 0;
const int CARTOON = 2 << 0;
const int DEPTH = 3 << 0;

class ModuleShader : public Module
{
public:
	ModuleShader();
	virtual ~ModuleShader();

	bool Init() override;

	GLuint GetProgramId(int flags, const std::string& name) const;

private:

	uint AddShaderPath(const char* path, GLenum shaderType, const char* defines = "");
	uint AddShader(const char* source, GLenum shaderType, const char* defines = "");
	
	uint AddProgram(const std::list<uint>& shaders);
	uint AddProgram(std::initializer_list<uint> shaders);

	// GLuint GetShaderId(ShaderType shaderType) const;

// private:

	/* void SetUpColorProgram();
	

	void SetUpDiffuseShader(); */

	void SetUpDefaultShader();
	void SetUpLightningShader();
	void SetUpCartoonShader();
	void SetUpDepthShader();

	void SetUpUberShader();

private:

	/* GLuint cartoonShaderId = 0;
	GLuint defaultShaderId = 0;
	GLuint colorShaderId = 0;
	GLuint depthShaderId = 0;
	GLuint diffuseShaderId = 0;

	GLuint uberShaderId = 0; */

	std::map<std::pair<int, std::string>, GLuint> shaders;
};

#endif