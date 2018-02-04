#include "SkyBox.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleTexture.h"

#include <DevIL.h>

//https://www.keithlantz.net/2011/10/rendering-a-skybox-using-a-cube-map-with-opengl-and-glsl/

SkyBox::SkyBox()
{
}


SkyBox::~SkyBox()
{
}

void SkyBox::SetUp()
{
	LoadShaders();
	LoadVerticesAndIndices();
	LoadTextures();
}

void SkyBox::LoadVerticesAndIndices()
{
	// cube vertices for vertex buffer object
	GLfloat cube_vertices[] = {
		-1.0,  1.0,  1.0,
		-1.0, -1.0,  1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
	};
	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	// cube indices for index buffer object
	GLushort cube_indices[] = {
		0, 1, 2, 3,
		3, 2, 6, 7,
		7, 6, 5, 4,
		4, 5, 1, 0,
		0, 3, 7, 4,
		1, 2, 6, 5,
	};
	glGenBuffers(1, &ibo_cube_indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SkyBox::LoadTextures()
{
	//Create Skybox Texture in memory and set properties
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Load textures from disk
	App->texture->LoadTextureGeneric("Assets/rock.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	App->texture->LoadTextureGeneric("Assets/rock.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	App->texture->LoadTextureGeneric("Assets/rock.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	App->texture->LoadTextureGeneric("Assets/rock.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	App->texture->LoadTextureGeneric("Assets/rock.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	App->texture->LoadTextureGeneric("Assets/rock.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	glEnableVertexAttribArray(vbo_cube_vertices);
	glVertexAttribPointer(vbo_cube_vertices, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SkyBox::CleanUp()
{
	glDeleteTextures(1, &skyboxTexture);
}

void SkyBox::Draw()
{
	glUseProgram(skyboxProgram);
	
	float4x4 m = App->camera->GetProyectionMatrix() * App->camera->GetViewMatrix();
	glUniformMatrix4fv(skyboxProgram, 1, GL_FALSE, m.Transposed().ptr());

	glDrawElements(GL_QUADS, 36, GL_UNSIGNED_SHORT, 0);

	glUseProgram(0);
}

void SkyBox::LoadShaders()
{
	uint vertex = App->shader->AddShader("Shader/skybox.vert", GL_VERTEX_SHADER);
	uint fragment = App->shader->AddShader("Shader/skybox.frag", GL_FRAGMENT_SHADER);
	skyboxProgram = App->shader->AddProgram({ vertex, fragment });
}