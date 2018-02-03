#include "Material.h"

#include <fstream>

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleShader.h"
#include "Shader.h"
#include "GameObject.h"

using namespace std;

Material::Material():Component(ComponentType::Material)
{
	AddShader("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	AddShader("Assets/Shaders/fragmentShader.frag", GL_FRAGMENT_SHADER);
}

Material::~Material()
{

}

void Material::DrawUI()
{
	if(ImGui::CollapsingHeader("Material"))
	{
		static char buff[252] = "";
		ImGui::InputText("Texture", buff, 252);
		if(ImGui::Button("Set Texture"))
		{
			string s = "Assets/";
			s += buff;

			SetTexture(s.c_str());
		}
		static char buff2[252] = "";
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
	textureId = App->renderer->LoadTexture(path, nullptr);
}

uint Material::AddShader(const char * path, GLenum shaderType)
{
	uint id = 0;
	ifstream t(path);
	if(t.good())
	{
		string str((std::istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		id = App->shader->AddShader(str.c_str(), shaderType)->GetId();
	}

	t.close();

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
