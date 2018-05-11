#ifndef _MODULESHADER_H_
#define _MODULESHADER_H_

#include <GL/glew.h>
#include <list>
#include <map>
#include <initializer_list>

#include "Module.h"

enum class ShaderType
{
	Default,
	Lightning,
	Cartoon,
	Depth,
	Color,
	Rigging,
	NormalMap,
	NormalMapRigging
};

const int DEFAULT = 0;
const int LIGHTNING = 1 << 0;
const int CARTOON = 2 << 0;
const int DEPTH = 3 << 0;
const int COLOR = 4 << 0;
const int RIGGING = 5 << 0;
const int NORMALMAP = 6 << 0;
const int NORMALMAPRIGGING = 7 << 0;

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

	void SetUpDefaultShader();
	void SetUpLightningShader();
	void SetUpCartoonShader();
	void SetUpDepthShader();
	void SetUpColorProgram();
	void SetUpRiggingProgram();
	void SetUpNormalMapProgram();
	void SetUpNormalMapRiggingProgram();

	void SetUpUberShader();

private:
	std::map<std::pair<int, std::string>, GLuint> shaders;
};

#endif