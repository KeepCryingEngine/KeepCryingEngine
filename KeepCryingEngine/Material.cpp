#include "Material.h"

#include <fstream>

#include "Application.h"
#include "ModuleTexture.h"
#include "ModuleShader.h"

using namespace std;

Material::Material():Component(ComponentType::Material)
{
	AddShader("Assets/Shaders/vertexShader.vert", GL_VERTEX_SHADER);
	AddShader("Assets/Shaders/fragmentShader.frag", GL_FRAGMENT_SHADER);

	textureId = App->texture->LoadCheckerTexture();
}

Material::~Material()
{ }

void Material::DrawUI()
{
	if(ImGui::CollapsingHeader("Material"))
	{
		static char materialBuffer[252] = {};
		ImGui::InputText("Name", materialBuffer, 252);
		if(ImGui::Button("Set Texture"))
		{
			string s = "Assets/";
			s += materialBuffer;

			SetTexture(s.c_str());
		}

		ImGui::NewLine();

		static char buff2[252] = {};
		static int shaderMode = 0;
		ImGui::InputText("Name", buff2, 252);
		ImGui::Combo("Type", &shaderMode, "Vertex\0Fragment");
		if(ImGui::Button("Add Shader"))
		{
			switch(shaderMode)
			{
				case 0:
				{
					shaders.push_back(AddShader(buff2, GL_VERTEX_SHADER));
				}
				break;
				case 1:
				{
					shaders.push_back(AddShader(buff2, GL_FRAGMENT_SHADER));
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
	textureId = App->texture->LoadTexture(path);
}

uint Material::AddShader(const char* path, GLenum shaderType)
{
	uint id = 0;
	ifstream t(path);
	if(t.good())
	{
		string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		id = App->shader->AddShader(str.c_str(), shaderType);
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