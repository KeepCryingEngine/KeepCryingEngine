#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <json.hpp>
#include <GL/glew.h>
#include <experimental/filesystem>

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
	Texture* GetTextureNormalMap()const;
	void SetTexture(Texture* texture);
	void SetTextureByPath(const std::experimental::filesystem::path& path);
	void SetTextureNormalMap(Texture* texture);
	void SetTextureNormalMapByPath(const std::experimental::filesystem::path& path);
	ShaderType GetShaderType() const;
	void SetShaderType(ShaderType shaderType);

private:
	ShaderType shaderType = ShaderType::Default;
	GLuint programId = 0;
	Texture* texture = nullptr;
	Texture* normalMap = nullptr;

	static const float TEXTURE_WINDOW_WIDTH_PERCENTAGE;
	static const float TEXTURE_WINDOW_HEIGHT_PERCENTAGE;
};

#endif // !_MATERIALENTITY_H_