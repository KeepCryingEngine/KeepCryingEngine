#include "Material.h"

#include <string>

#include <imgui.h>

#include "Application.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"
#include "Texture.h"

using namespace std;

Material::Material()
{
	texture = App->texture->GetCheckerTexture();
	programId = App->shader->GetProgramId(DEFAULT, "UberShader");
}

Material::Material(const Material & mat) :
	shaderType(mat.shaderType),
	programId(mat.programId),
	texture(mat.texture)
{
	App->texture->Subscribe(texture);
}

Material::~Material()
{
	App->texture->Release(texture);
}

void Material::DrawUI()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		static char materialBuffer[252] = {};
		ImGui::InputText("##setTexture", materialBuffer, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			string s = "Assets/";
			s += materialBuffer;
		
			SetTextureByPath(s.c_str());
		}
		
		ImGui::NewLine();
		
		int tmpShaderMode = (int)shaderType;
		if(ImGui::Combo("Shader", &tmpShaderMode, "Default\0Lightning\0Cartoon\0Depth\0Color\0Rigging\0NormalMap\0NormalMapRigging"))
		{
			SetShaderType((ShaderType)tmpShaderMode);
		}
		if(shaderType == ShaderType::NormalMap || shaderType == ShaderType::NormalMapRigging)
		{
			static char materialNormalBuffer[252] = {};
			ImGui::InputText("##setTextureNormalMap", materialNormalBuffer, 252); ImGui::SameLine();
			if(ImGui::Button("Set Texture##2"))
			{
				string s = "Assets/";
				s += materialNormalBuffer;

				SetTextureNormalMapByPath(s.c_str());
			}
		}
	}
}

GLuint Material::GetProgramId() const
{
	return programId;
}

Texture* Material::GetTexture() const
{
	return texture;
}

Texture * Material::GetTextureNormalMap() const
{
	return normalMap;
}

void Material::SetTexture(Texture * texture)
{
	assert(texture != nullptr);
	if (this->texture != nullptr)
	{
		App->texture->Release(this->texture);
	}
	this->texture = texture;
}

void Material::SetTextureByPath(const std::experimental::filesystem::path& path)
{
	TextureIdentifier textureIdentifier = { path };
	Texture * texture = App->texture->GetAsset(textureIdentifier);
	if(texture != nullptr)
	{
		App->texture->Release(this->texture);
		this->texture = texture;
	}
}

void Material::SetTextureNormalMap(Texture * texture)
{
	assert(texture != nullptr);
	if(this->normalMap != nullptr)
	{
		App->texture->Release(this->normalMap);
	}
	this->normalMap = texture;
}

void Material::SetTextureNormalMapByPath(const std::experimental::filesystem::path & path)
{
	TextureIdentifier textureIdentifier = { path };
	Texture * texture = App->texture->GetAsset(textureIdentifier);
	if(texture != nullptr)
	{
		if(normalMap != nullptr)
		{
			App->texture->Release(this->normalMap);
		}
		this->normalMap = texture;
	}
}

ShaderType Material::GetShaderType() const
{
	return shaderType;
}

void Material::SetShaderType(ShaderType shaderType)
{
	this->shaderType = shaderType;

	int flags = 0;
	string name = "";

	switch(shaderType)
	{
		case ShaderType::Default:
			flags |= DEFAULT;
			name = "UberShader";
			break;
		case ShaderType::Lightning:
			flags |= LIGHTNING;
			name = "UberShader";
			break;
		case ShaderType::Cartoon:
			flags |= CARTOON;
			name = "UberShader";
			break;
		case ShaderType::Depth:
			flags |= DEPTH;
			name = "UberShader";
			break;
		case ShaderType::Color:
			flags |= COLOR;
			name = "UberShader";
			break;
		case ShaderType::Rigging:
			flags |= RIGGING;
			name = "UberShader";
			break;
		case ShaderType::NormalMap:
			flags |= NORMALMAP;
			name = "UberShader";
			break;
		case ShaderType::NormalMapRigging:
			flags |= NORMALMAPRIGGING;
			name = "UberShader";
			break;
	}

	programId = App->shader->GetProgramId(flags, name);
}