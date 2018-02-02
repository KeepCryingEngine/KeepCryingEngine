#include "ModuleShader.h"

#include "Shader.h"
#include <fstream>

using namespace std;

ModuleShader::ModuleShader()
{}

ModuleShader::~ModuleShader()
{}

bool ModuleShader::Init()
{
	const Shader* defaultVertexShader = nullptr;
	const Shader* defaultFragmentShader = nullptr;

	std::ifstream t(".\\Assets\\Shaders\\vertexShader.vert");
	if(t.good())
	{
		std::string vertex((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		defaultVertexShader = AddShader(vertex.c_str(), GL_VERTEX_SHADER);
	}
	t.close();

	t.open(".\\Assets\\Shaders\\fragmentShader.frag");
	if(t.good())
	{
		std::string fragment((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		defaultFragmentShader = AddShader(fragment.c_str(), GL_FRAGMENT_SHADER);
	}
	t.close();

	defaultProgramId = AddProgram({ defaultVertexShader, defaultFragmentShader });

	return true;
}

bool ModuleShader::Start()
{
	//glUseProgram(defaultProgramId);

	return true;
}

const Shader* ModuleShader::AddShader(const char* source, GLenum shaderType)
{
	GLuint shaderId = glCreateShader(shaderType);

	glShaderSource(shaderId, 1, &source, nullptr);
	glCompileShader(shaderId);

	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
		LOG_DEBUG("Shader compilation error: %s", infoLog);

		return nullptr;
	}

	return new Shader(shaderId);

	// glDeleteShader(shaderId);
}

uint ModuleShader::AddProgram(initializer_list<const Shader*> shaders)
{
	GLuint shaderProgramId = glCreateProgram();
	
	for(const Shader* shader : shaders)
	{
		if(shader)
		{
			glAttachShader(shaderProgramId, shader->GetId());
		}
	}

	glLinkProgram(shaderProgramId);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(shaderProgramId, 512, nullptr, infoLog);
		LOG_DEBUG("Shader link error: %s", infoLog);

		return 0;
	}

	return shaderProgramId;
}

uint ModuleShader::AddProgram(std::list<uint> shaders)
{
	GLuint shaderProgramId = glCreateProgram();

	for(uint shader : shaders)
	{
		glAttachShader(shaderProgramId, shader);
	}

	glLinkProgram(shaderProgramId);

	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(shaderProgramId, 512, nullptr, infoLog);
		LOG_DEBUG("Shader link error: %s", infoLog);

		return 0;
	}

	return shaderProgramId;
}
