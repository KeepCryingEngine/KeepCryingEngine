#include "Material.h"

#include <fstream>

#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleShader.h"

using namespace std;

Material::Material():Component(ComponentType::Material)
{
	SetUpDefaultShader();
	textureId = App->texture->LoadCheckerTexture();
}

Material::~Material()
{ }

void Material::DrawUI()
{
	if(ImGui::CollapsingHeader("Material"))
	{
		static char materialBuffer[252] = {};
		ImGui::InputText("##setTexture", materialBuffer, 252); ImGui::SameLine();
		if(ImGui::Button("Set Texture"))
		{
			string s = "Assets/";
			s += materialBuffer;

			SetTexture(s.c_str());
		}

		ImGui::NewLine();

		int tmpShaderMode = (int)shaderMode;

		if(ImGui::Combo("Shader", &tmpShaderMode, "Default\0Cartoon\0Depth"))
		{
			shaderMode = (ShaderMode)tmpShaderMode;
			switch(shaderMode)
			{
				case ShaderMode::DEFAULT:
				{
					SetUpDefaultShader();
				}
					break;
				case ShaderMode::CARTOON:
				{
					SetUpCartoonShader();
				}
					break;
				case ShaderMode::DEPTH:
				{
					SetUpDepthShader();
				}
					break;
			}
		}
	}
}

uint Material::GetTextureId()
{
	return textureId;
}

uint Material::GetProgramId()
{
	return programId;
}

vector<ComponentType> Material::GetNeededComponents() const
{
	return { ComponentType::Transform, ComponentType::Mesh };
}

vector<ComponentType> Material::GetProhibitedComponents() const
{
	return { ComponentType::Material };
}

void Material::SetTexture(const char* path)
{
	uint newTextureID = App->texture->LoadTexture(path);
	if(newTextureID != 0)
	{
		textureId = newTextureID;
	}
}

uint Material::AddShader(const char* path, GLenum shaderType)
{
	return App->shader->AddShaderPath(path,shaderType);
}

void Material::SetUpDefaultShader()
{
	uint vertexId =AddShader("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShader("Assets/Shaders/fragmentShader.frag", GL_FRAGMENT_SHADER);
	programId = App->shader->AddProgram({ vertexId, fragmentId });
}

void Material::SetUpCartoonShader()
{
	uint vertexId = AddShader("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShader("Assets/Shaders/cartoon.frag", GL_FRAGMENT_SHADER);
	programId = App->shader->AddProgram({ vertexId, fragmentId });
}

void Material::SetUpDepthShader()
{
	uint vertexId = AddShader("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	uint fragmentId = AddShader("Assets/Shaders/depthShader.frag", GL_FRAGMENT_SHADER);
	programId = App->shader->AddProgram({ vertexId, fragmentId });
}
