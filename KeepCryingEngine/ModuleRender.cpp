#include "ModuleRender.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include <DevIL.h>

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "Camera.h"
#include "GameObject.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"

const float3 ModuleRender::LIGHT_DIR = { -1.0,1.0,0.0 };

using namespace std;

ModuleRender::ModuleRender()
{}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	LOG_DEBUG("Creating Renderer context");
	bool ret = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	glcontext = SDL_GL_CreateContext(App->window->window);

	LOG_DEBUG("Using Glew %s", glewGetString(GLEW_VERSION));
	LOG_DEBUG("Vendor: %s", glGetString(GL_VENDOR));
	LOG_DEBUG("Renderer: %s", glGetString(GL_RENDERER));
	LOG_DEBUG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG_DEBUG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	//Antialiasing
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetUpLight();

	GLenum err = glewInit();

	if(GLEW_OK != err)
	{
		LOG_DEBUG("GLEW could not initialize! GLEW_Error: %s", glewGetErrorString(err));
		ret = false;
	}

	//Initialize DevIL
	ilInit();
	iluInit();
	ilutInit();

	if(!ilutRenderer(ILUT_OPENGL))
	{
		LOG_DEBUG("DevIL could not initialize!");
		ret = false;
	}

	return ret;
}

update_status ModuleRender::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->camera->camera->GetProyectionMatrix().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->camera->GetViewMatrix().ptr());
	
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::Update(float deltaTimeS, float realDeltaTimeS)
{
	DrawGrid();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{
	for(Mesh* mesh : drawBuffer)
	{
		DrawFromBuffer(*mesh);
	}
	drawBuffer.clear();

	SDL_GL_SwapWindow(App->window->window);

	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	SDL_GL_DeleteContext(glcontext);

	return true;
}

void ModuleRender::DrawGrid() const
{
	glPushMatrix();

	GLfloat previousColor[4];
	glGetFloatv(GL_CURRENT_COLOR, previousColor);

	glBegin(GL_LINES);

	glColor3f(220, 220, 220);

	float step = 1.0f;
	float length = 1000.0f;

	for (float r = -length; r <= length; r += step)
	{
		glVertex3f(-length, 0, r);
		glVertex3f(length, 0, r);

		glVertex3f(r, 0, -length);
		glVertex3f(r, 0, length);
	}

	glEnd();

	glColor3f(previousColor[0], previousColor[1], previousColor[2]);
	glPopMatrix();
}

void ModuleRender::DrawCross(const float3& pos,float scale)const
{
	float crossSize = 0.01f * scale;
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
	glLineWidth(3);

	GLfloat previousColor[4];
	glGetFloatv(GL_CURRENT_COLOR, previousColor);

	glBegin(GL_LINES);

	glColor3f(255, 0, 0);

	glVertex3f(-crossSize, 0, 0);
	glVertex3f(crossSize, 0, 0);

	glVertex3f(0, -crossSize, 0);
	glVertex3f(0, crossSize, 0);

	glEnd();

	glColor3f(previousColor[0], previousColor[1], previousColor[2]);
	glLineWidth(1);

	glPopMatrix();
}

void ModuleRender::AddToDrawBuffer(Mesh & mesh)
{
	drawBuffer.push_back(&mesh);
}

void ModuleRender::DrawFrustrum(Camera & camera)
{
	float lineWidth;

	glGetFloatv(GL_LINE_WIDTH, &lineWidth);

	glLineWidth(5.0f);

	uint progId = App->shader->cameraProgramId;
	glUseProgram(progId);

	glBindBuffer(GL_ARRAY_BUFFER, camera.GetFrustumBufferId());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	GLint modelView = glGetUniformLocation(progId, "model_view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->camera->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->camera->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "transform");
	Transform* transform = (Transform*)camera.gameObject->GetComponent(ComponentType::Transform);
	float4x4 transformMatrix = transform->GetModelMatrix();
	transformMatrix.RemoveScale();
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, transformMatrix.Transposed().ptr());

	GLint width = glGetUniformLocation(progId, "ourColor");
	glUniform4f(width, 1.0f, 0.0f, 0.0f, 1.0f);

	glDrawArrays(GL_LINES, 0, camera.GetNumberOfPoints());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableVertexAttribArray(0);

	glUseProgram(0);

	glLineWidth(lineWidth);
}

void ModuleRender::SetUpLight() const
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	glEnable(GL_LIGHTING);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void ModuleRender::DrawFromBuffer(Mesh& mesh)
{
	Material* material = (Material*)(mesh.gameObject->GetComponent(ComponentType::Material));
	uint progId = material->GetProgramId();
	uint textId = material->GetTextureId();

	glUseProgram(progId);
	if(textId != 0)
	{
		GLint texture = glGetUniformLocation(progId, "ourTexture");
		glUniform1i(texture, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textId);
	}

	glBindBuffer(GL_ARRAY_BUFFER, mesh.GetVertexBufferId());

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.GetNormalBufferId());
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLint modelView = glGetUniformLocation(progId, "model_view");
	glUniformMatrix4fv(modelView, 1, GL_FALSE, App->camera->camera->GetViewMatrix().ptr());

	GLint proyection = glGetUniformLocation(progId, "projection");
	glUniformMatrix4fv(proyection, 1, GL_FALSE, App->camera->camera->GetProyectionMatrix().ptr());

	GLint transformUniformId = glGetUniformLocation(progId, "transform");
	Transform* transform = ((Transform*)mesh.gameObject->GetComponent(ComponentType::Transform));
	glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, transform->GetModelMatrix().Transposed().ptr());

	GLint light = glGetUniformLocation(progId, "lightDir");
	if(light != -1)
	{
		glUniform3f(light, LIGHT_DIR.x, LIGHT_DIR.y, LIGHT_DIR.z);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.GetIndicesBufferId());
	glDrawElements(mesh.GetDrawMode(), mesh.GetVerticesNumber(), GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glUseProgram(0);
}
