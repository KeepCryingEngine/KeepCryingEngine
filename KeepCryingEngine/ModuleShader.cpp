#include "ModuleShader.h"

#include "Shader.h"

const char* SHADER_0 = "#version 330 core\n\
layout (location = 0) in vec3 position;\
layout (location = 1) in vec4 color;\
layout (location = 2) in vec2 texCoord;\
\
out vec4 ourColor;\
out vec2 TexCoord;\
\
uniform mat4 model_view;\
uniform mat4 projection;\
\
void main()\
{\
	gl_Position = projection * model_view * vec4(position, 1.0f);\
	ourColor = color;\
	TexCoord = texCoord;\
}\
";

const char* SHADER_1 = "#version 330 core\n\
in vec4 ourColor;\
in vec2 TexCoord;\
\
out vec4 color;\
\
uniform sampler2D ourTexture;\
\
void main()\
{\
	color = texture2D(ourTexture,TexCoord);\
}\
";

ModuleShader::ModuleShader()
{}


ModuleShader::~ModuleShader()
{}

bool ModuleShader::Init()
{
	const Shader* defaultVertexShader = AddShader(SHADER_0, GL_VERTEX_SHADER);
	const Shader* defaultFragmentShader = AddShader(SHADER_1, GL_FRAGMENT_SHADER);

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

uint ModuleShader::AddProgram(std::initializer_list<const Shader*> shaders)
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