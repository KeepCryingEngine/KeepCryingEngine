#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Component.h"

#include <GL/glew.h>
#include <list>

#include "Globals.h"

enum class ShaderMode
{
	DEFAULT,
	CARTOON
};

class Material : public Component
{
public:
	Material();
	virtual ~Material();

	void DrawUI() override;

	uint GetTextureId();
	uint GetProgramId();

	virtual std::vector<ComponentType> GetNeededComponents() const override;
	virtual std::vector<ComponentType> GetProhibitedComponents() const override;

private:
	void SetTexture(const char* path);
	uint AddShader(const char* path, GLenum shaderType);

private:
	void SetUpDefaultShader();
	void SetUpCartoonShader();
	

private:
	uint textureId;
	uint programId;
	ShaderMode shaderMode = ShaderMode::DEFAULT;
};

#endif // !_MATERIAL_H_

