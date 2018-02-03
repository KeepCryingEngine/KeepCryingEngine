#include "Material.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleShader.h"
#include "Shader.h"
#include "GameObject.h"

using namespace std;

Material::Material():Component(ComponentType::Material)
{
	AddShader("Assets/Shaders/vertexShader.vert",GL_VERTEX_SHADER);
	AddShader("Assets/Shaders/fragmentShader.frag", GL_FRAGMENT_SHADER);
}

Material::~Material()
{

}

void Material::DrawUI()
{
	ImGui::Checkbox("Active",&enabled);
	if(ImGui::Button("Delete Component"))
	{
		gameObject->RemoveComponent(this);
	}
	char* buff;
	ImGui::InputText("Texture", buff, 252);
	if(ImGui::Button("Set Texture"))
	{
		SetTexture(buff);
	}
	char* buff2;
	static int shaderMode = 0;
	ImGui::InputText("Shader", buff2, 252);
	ImGui::Combo("Type", &shaderMode, "Vertex\0Fragment");
	if(ImGui::Button("Add Shader"))
	{
		switch(shaderMode)
		{
			case 0:
			{
				AddShader(buff2, GL_VERTEX_SHADER);
			}
				break;
			case 1:
			{
				AddShader(buff2, GL_FRAGMENT_SHADER);
			}
			break;
		}
	}
	//TODO make a delete shader button with the id that give addbuffer ^
}

uint Material::GetTextureId()
{
	return textureId;
}

uint Material::GetProgramId()
{
	return programId;
}

void Material::SetTexture(const char * path)
{
	textureId = App->renderer->LoadTexture(path,nullptr);
}

uint Material::AddShader(const char * path, GLenum shaderType)
{
	uint id=App->shader->AddShader(path,shaderType)->GetId();
	shaders.push_back(id);
	BuildProgram();
	return id;
}

void Material::RemoveShader(uint id)
{
	shaders.remove(id);
	BuildProgram();
}

void Material::BuildProgram()
{
	programId = App->shader->AddProgram(shaders);
}
