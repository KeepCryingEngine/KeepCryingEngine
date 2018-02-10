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
	programId = App->shader->GetShaderId(ShaderType::Default);
}

Material::~Material()
{
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
		
			SetTexture(s.c_str());
		}
		
		ImGui::NewLine();
		
		int tmpShaderMode = (int)shaderType;
		if(ImGui::Combo("Shader", &tmpShaderMode, "Default\0Cartoon"))
		{
			shaderType = (ShaderType)tmpShaderMode;
			programId = App->shader->GetShaderId(shaderType);
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

void Material::SetTexture(const char* path)
{
	Texture * texture = App->texture->LoadTexture(path);
	if(texture)
	{
		this->texture = texture;
	}
}