#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Component.h"

#include <GL/glew.h>
#include <list>

#include "Globals.h"

class Material : Component
{
public:
	Material();
	~Material();

	void DrawUI() override;

	uint GetTextureId();
	uint GetProgramId();

private:
	void SetTexture(const char* path);
	uint AddShader(const char* path, GLenum shaderType);
	void RemoveShader(uint id);

private:
	void BuildProgram();
	

private:
	uint textureId;
	uint programId;
	std::list<uint> shaders;

};

#endif // !_MATERIAL_H_

