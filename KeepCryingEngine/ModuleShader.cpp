#include "ModuleShader.h"

#include <fstream>

using namespace std;

ModuleShader::ModuleShader()
{}

ModuleShader::~ModuleShader()
{}

bool ModuleShader::Init()
{
	uint defaultVertexShader = 0;
	uint defaultFragmentShader = 0;

	ifstream t(".\\Assets\\Shaders\\vertexShader.vert");
	if(t.good())
	{
		string vertex((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		defaultVertexShader = AddShader(vertex.c_str(), GL_VERTEX_SHADER);
	}
	t.close();

	t.open(".\\Assets\\Shaders\\fragmentShader.frag");
	if(t.good())
	{
		string fragment((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		defaultFragmentShader = AddShader(fragment.c_str(), GL_FRAGMENT_SHADER);
	}
	t.close();

	defaultProgramId = AddProgram({ defaultVertexShader, defaultFragmentShader });

	return true;
}

uint ModuleShader::AddShader(const char* source, GLenum shaderType)
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

		return 0;
	}

	return shaderId;
}

uint ModuleShader::AddProgram(initializer_list<uint> shaders)
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

uint ModuleShader::AddProgram(const list<uint>& shaders)
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