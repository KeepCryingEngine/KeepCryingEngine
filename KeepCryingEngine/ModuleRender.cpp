#include "ModuleRender.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include "GL/glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Application.h"
#include "ModuleWindow.h"

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
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	GLenum err = glewInit();

	setupBigArray();
	setupIndicesArray();
	setUpSphere(0.5,100,100);

	return ret;
}

update_status ModuleRender::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{

	glRotatef(20.0f * deltaTimeS, 0.0f, 1.0f, 0.2f);

	//drawCubeDirect(0,0,0);
	//drawCubeBigArray(0,0,0);
	//drawCubeIndices(0,0,0);
	//drawSphere(0, 0, 0);

	SDL_GL_SwapWindow(App->window->window);
	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	SDL_GL_DeleteContext(glcontext);
	return true;
}

void ModuleRender::setupBigArray() const
{
	float half = 0.5f;

	float vertices[36 * 3] = {
		-half, -half, -half,
		half, -half, -half,
		-half, half, -half,
		half, -half, -half,
		half, half, -half,
		-half, half, -half,
		half, -half, -half,
		half, -half, half,
		half, half, -half,
		half, -half, half,
		half, half, half,
		half, half, -half,
		-half, half, -half,
		half, half, -half,
		-half, half, half,
		half, half, -half,
		half, half, half,
		-half, half, half,
		-half, -half, half,
		-half, half, half,
		half, -half, half,
		half, -half, half,
		-half, half, half,
		half, half, half,
		-half, -half, -half,
		-half, half, -half,
		-half, -half, half,
		-half, -half, half,
		-half, half, -half,
		-half, half, half,
		-half, -half, -half,
		-half, -half, half,
		half, -half, -half,
		half, -half, -half,
		-half, -half, half,
		half, -half, half
	};

	glGenBuffers(1, (GLuint*) &(bigArrayCube));
	glBindBuffer(GL_ARRAY_BUFFER, bigArrayCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRender::setupIndicesArray() const
{
	float half = 0.5f;
	float uniqueVertex[8 * 3] = {
		-0.5f, 0.0f, 0.0f, //0
		0.5f, 0.0f, 0.0f,  //1
		0.5f, 0.5f, 0.0f,  //2
		-0.5f, 0.5f, 0.0f, //3
		-0.5f, 0.0f, 0.5f, //4
		0.5f, 0.0f, 0.5f,  //5
		0.5f, 0.5f, 0.5f,  //6
		-0.5f, 0.5f, 0.5f  //7
	};

	glGenBuffers(1, (GLuint*) &(vertexArrayBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, uniqueVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint indicesArray[36 * 3] = {
		0, 1, 3,
		1, 2, 3,

		1, 5, 2,
		5, 6, 2,

		5, 4, 6,
		4, 7, 6,

		4, 0, 7,
		0, 3, 7,

		3, 2, 7,
		2, 6, 7,

		4, 5, 0,
		5, 1, 0
	};

	glGenBuffers(1, (GLuint*) &(indicesArrayBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesArrayBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36 * 3, indicesArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleRender::setUpSphere(float radius, unsigned int rings, unsigned int sectors) 
{
	float R = 1.f / (float)(rings - 1);
	float S = 1.f / (float)(sectors - 1);
	// int r, s;

	vector<GLfloat>vertices;
	//vector<GLfloat>normals;
	//vector<GLfloat>textcord;
	vector<GLushort>indices;

	vertices.resize(rings * sectors * 3);
	//normals.resize(rings * sectors * 3);
	//texcoords.resize(rings * sectors * 2);
	vector<GLfloat>::iterator v = vertices.begin();
	//vector<GLfloat>::iterator n = normals.begin();
	//vector<GLfloat>::iterator t = texcoords.begin();
	for(unsigned int r = 0; r < rings; r++) for(unsigned int s = 0; s < sectors; s++)
	{
		//Not sure about those formulas:Adrian
		float y = (float)sin(-M_PI_2 + M_PI * r * R);
		float x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
		float z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

		//*t++ = s * S;
		//*t++ = r * R;

		*v++ = x * radius;
		*v++ = y * radius;
		*v++ = z * radius;
		//Not sure about normal vector being filled with points like this:Adrian
		//*n++ = x;
		//*n++ = y;
		//*n++ = z;
	}

	glGenBuffers(1, (GLuint*) &(sphereVertex));
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * rings * sectors * 3, ((void*)&vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indices.resize(rings * sectors * 4);
	sphereIndicesSize = indices.size();
	std::vector<GLushort>::iterator i = indices.begin();
	for(unsigned int r = 0; r < rings - 1; r++) for(unsigned int s = 0; s < sectors - 1; s++)
	{
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}
	glGenBuffers(1, (GLuint*) &(sphereIndex));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) *sphereIndicesSize, ((void*)&indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleRender::drawCubeDirect(float x, float y, float z) const
{
	float half = 0.5f;

	glBegin(GL_TRIANGLES);

	glColor3f(100.0f, 0.0f, 0.0f);
	glVertex3f(-half, -half, -half);
	glVertex3f(half, -half, -half);
	glVertex3f(-half, half, -half);
	glVertex3f(half, -half, -half);
	glVertex3f(half, half, -half);
	glVertex3f(-half, half, -half);

	glColor3f(0.0f, 100.0f, 0.0f);
	glVertex3f(half, -half, -half);
	glVertex3f(half, -half, half);
	glVertex3f(half, half, -half);
	glVertex3f(half, -half, half);
	glVertex3f(half, half, half);
	glVertex3f(half, half, -half);

	glColor3f(0.0f, 0.0f, 100.0f);
	glVertex3f(-half, half, -half);
	glVertex3f(half, half, -half);
	glVertex3f(-half, half, half);
	glVertex3f(half, half, -half);
	glVertex3f(half, half, half);
	glVertex3f(-half, half, half);

	glColor3f(100.0f, 0.0f, 0.0f);
	glVertex3f(-half, -half, half);
	glVertex3f(-half, half, half);
	glVertex3f(half, -half, half);
	glVertex3f(half, -half, half);
	glVertex3f(-half, half, half);
	glVertex3f(half, half, half);

	glColor3f(0.0f, 100.0f, 0.0f);
	glVertex3f(-half, -half, -half);
	glVertex3f(-half, half, -half);
	glVertex3f(-half, -half, half);
	glVertex3f(-half, -half, half);
	glVertex3f(-half, half, -half);
	glVertex3f(-half, half, half);

	glColor3f(0.0f, 0.0f, 100.0f);
	glVertex3f(-half, -half, -half);
	glVertex3f(-half, -half, half);
	glVertex3f(half, -half, -half);
	glVertex3f(half, -half, -half);
	glVertex3f(-half, -half, half);
	glVertex3f(half, -half, half);

	glTranslatef(x, y, z);

	glEnd();
}

void ModuleRender::drawCubeBigArray(float x, float y, float z) const
{
	glColor3f(255.0f, 0.0f, 1.0f);
	glTranslatef(x, y, z);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, bigArrayCube);

	glVertexPointer(3, GL_FLOAT, 0, nullptr);
	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRender::drawCubeIndices(float x, float y, float z) const
{
	glColor3f(255.0f, 0.0f, 1.0f);
	glTranslatef(x, y, z);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesArrayBuffer);
	glDrawElements(GL_TRIANGLES, 36 * 3, GL_UNSIGNED_INT, nullptr); //The nullptr means 'take the last binded buffer'

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRender::drawSphere(float x, float y, float z)const
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glColor3f(255.0f, 0.0f, 1.0f);
	glTranslatef(x, y, z);

	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_NORMAL_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//glNormalPointer(GL_FLOAT, 0, &normals[0]);
	//glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndex);
	glDrawElements(GL_QUADS, sphereIndicesSize, GL_UNSIGNED_SHORT, NULL);
}
