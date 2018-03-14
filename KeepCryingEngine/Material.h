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
	void SetTexture(const std::experimental::filesystem::path&);

	void Load(const nlohmann::json& json);
	void Save(nlohmann::json& json) const;

private:
	std::experimental::filesystem::path path;
	ShaderType shaderType = ShaderType::Default;
	GLuint programId = 0;
	Texture* texture = nullptr;
};

#endif // !_MATERIALENTITY_H_