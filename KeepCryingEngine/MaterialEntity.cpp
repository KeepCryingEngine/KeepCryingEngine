#include "MaterialEntity.h"

#include <string>

#include <imgui.h>

#include "Application.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"

using namespace std;

MaterialEntity::MaterialEntity()
{
}

MaterialEntity::~MaterialEntity()
{
}

void MaterialEntity::DrawUI()
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

GLuint MaterialEntity::GetProgramId() const
{
	return programId;
}

GLuint MaterialEntity::GetTextureId() const
{
	return textureId;
}

void MaterialEntity::SetTexture(const char* path)
{
	uint newTextureID = App->texture->LoadTexture(path);
	if(newTextureID != 0)
	{
		textureId = newTextureID;
	}
}