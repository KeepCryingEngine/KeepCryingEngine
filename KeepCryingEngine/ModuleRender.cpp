#include "ModuleRender.h"

#define _USE_MATH_DEFINES

#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <DevIL.h>

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"

using namespace std;

const uint ModuleRender::CHECKERS_HEIGHT = 128;
const uint ModuleRender::CHECKERS_WIDTH = 128;

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
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

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

	if (!ilutRenderer(ILUT_OPENGL))
	{
		LOG_DEBUG("DevIL could not initialize!");
		ret = false;
	}

	if (ret) 
	{
		wrapModeS = wrapModeT = GL_CLAMP;

		magFilterMode = minFilterMode = GL_LINEAR;

		SetUpBigArray();
		SetUpIndicesArray();
		SetUpSphere(0.25f, 100, 100);
		SetUpPlane();
		SetUpTextures();
	}

	return ret;
}

update_status ModuleRender::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->camera->GetProyectionMatrix().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix().ptr());
	
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::Update(float deltaTimeS, float realDeltaTimeS)
{
	DrawGrid();
	DrawCubeDirect(2.0f, 2.0f, 10.0f);
	DrawCubeBigArray(0.0f, 2.0f, 10.0f);
	DrawCubeIndices(-2.0f, 2.0f, 10.0f);
	DrawSphere(0.0f, 4.0f, 10.0f);
	DrawPlane(0.0f, -1.0f, 12.0f);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{

	SDL_GL_SwapWindow(App->window->window);

	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	CleanUpTextures();

	SDL_GL_DeleteContext(glcontext);

	return true;
}

void ModuleRender::SetUpBigArray()
{
	float half = 0.5f;

	float vertices[36 * 3] =
	{
		half, -half, half,
		-half, half, half,
		-half, -half, half,
		half, half, half,
		-half, half, half,
		half, -half, half,

		-half, half, -half,
		half, -half, -half,
		-half, -half, -half,
		-half, half, -half,
		half, half, -half,
		half, -half, -half,

		half, half, -half,
		half, -half, half,
		half, -half, -half,
		half, half, -half,
		half, half, half,
		half, -half, half,

		-half, half, half,
		half, half, -half,
		-half, half, -half,
		-half, half, half,
		half, half, half,
		half, half, -half,

		-half, -half, half,
		-half, half, -half,
		-half, -half, -half,
		-half, half, half,
		-half, half, -half,
		-half, -half, half,

		half, -half, -half,
		-half, -half, half,
		-half, -half, -half,
		half, -half, half,
		-half, -half, half,
		half, -half, -half,
	};

	glGenBuffers(1, (GLuint*) &(bigArrayCube));
	glBindBuffer(GL_ARRAY_BUFFER, bigArrayCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	float uv[36 * 2] =
	{
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	1.0f, 0.0f
	};

	glGenBuffers(1, (GLuint*) &(bigArrayCubeUV));
	glBindBuffer(GL_ARRAY_BUFFER, bigArrayCubeUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 2, uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRender::SetUpIndicesArray()
{
	float half = 0.5f;
	float uniqueVertex[8 * 3 * 3] =
	{
		-half, -half, -half, // 0 Front bottom left
		half, -half, -half,  // 1 Front bottom right
		half, half, -half,  // 2 Front top right
		-half, half, -half, // 3 Front top left

		-half, -half, half, // 4 Left bottom left
		-half, -half, -half, // 5 Left bottom right
		-half, half, -half, // 6 Left top right
		-half, half, half,  // 7 Left top left

		-half, half, half,  // 8 Back top left
		half, half, half,  // 9 Back top right
		half, -half, half,  // 10 Back bottom right
		-half, -half, half, // 11 Back bottom left

		half, -half, -half,  // 12 Right bottom left
		half, -half, half,  // 13 Right bottom right
		half, half, half,  // 14 Right top right
		half, half, -half,  // 15 Right top left

		-half, -half, half, // 16 Bottom top left
		half, -half, half,  // 17 Bottom top right
		half, -half, -half,  // 18 Bottom bottom right
		-half, -half, -half, // 19 Bottom bottom left

		-half, half, -half, // 20 Top bottom left
		half, half, -half,  // 21 Top bottom right
		half, half, half,  // 22 Top top right
		-half, half, half  // 23 Top top left
	};

	glGenBuffers(1, (GLuint*) &(vertexArrayBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3 * 3, uniqueVertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	uint indicesArray[36 * 3] =
	{
		3, 1, 0,
		3, 2, 1,

		7, 5, 4,
		7, 6, 5,

		11, 9, 8,
		11, 10, 9,

		15, 13, 12,
		15, 14, 13,

		19, 17, 16,
		19, 18, 17,

		23, 21, 20,
		23, 22, 21
	};

	glGenBuffers(1, (GLuint*) &(indicesArrayBuffer));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesArrayBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36 * 3, indicesArray, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float uv[24 * 2] =
	{
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		1.0f,1.0f,
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f,

		0.0f,1.0f,
		1.0f,1.0f,
		1.0f,0.0f,
		0.0f,0.0f,

		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
		0.0f,1.0f
	};

	glGenBuffers(1, (GLuint*) &(uvArrayBuffer));
	glBindBuffer(GL_ARRAY_BUFFER, uvArrayBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 2, uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void ModuleRender::SetUpSphere(float radius, unsigned int rings, unsigned int sectors) 
{
	float R = 1.0f / (float)(rings - 1);
	float S = 1.0f / (float)(sectors - 1);

	vector<GLfloat> vertices;
	vector<GLushort> indices;
	vector<GLfloat> texcoords;

	vertices.resize(rings * sectors * 3);
	texcoords.resize(rings * sectors * 2);
	vector<GLfloat>::iterator v = vertices.begin();
	vector<GLfloat>::iterator t = texcoords.begin();
	for(unsigned int r = 0; r < rings; r++)
	{
		for(unsigned int s = 0; s < sectors; s++)
		{
			float y = (float)sin(-M_PI_2 + M_PI * r * R);
			float x = (float)cos(2 * M_PI * s * S) * (float)sin(M_PI * r * R);
			float z = (float)sin(2 * M_PI * s * S) * (float)sin(M_PI * r * R);

			*t++ = s * S;
			*t++ = r * R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;
		}
	}

	glGenBuffers(1, (GLuint*) &(sphereVertex));
	glBindBuffer(GL_ARRAY_BUFFER, sphereVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * rings * sectors * 3, ((void*)&vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(sphereUV));
	glBindBuffer(GL_ARRAY_BUFFER, sphereUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * rings * sectors * 2, ((void*)&texcoords[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	indices.resize(rings * sectors * 4);
	sphereIndicesSize = indices.size();

	std::vector<GLushort>::iterator i = indices.begin();

	for(unsigned int r = 0; r < rings - 1; r++)
	{
		for(unsigned int s = 0; s < sectors - 1; s++)
		{
			*i++ = r * sectors + s;
			*i++ = r * sectors + s + 1;
			*i++ = (r + 1) * sectors + s + 1;
			*i++ = (r + 1) * sectors + s;
		}
	}
	glGenBuffers(1, (GLuint*) &(sphereIndex));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * sphereIndicesSize, ((void*)&indices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ModuleRender::SetUpPlane()
{
	float value = 2.0f;

	float vertices[6 * 3] =
	{
		-value, value, -value,
		value, -value, -value,
		-value, -value, -value,
		-value, value, -value,
		value, value, -value,
		value, -value, -value
	};

	glGenBuffers(1, (GLuint*) &(planeVertices));
	glBindBuffer(GL_ARRAY_BUFFER, planeVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 3, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	float value2 = 3.0f;

	float uv[6 * 2] =
	{
		0.0f, value2,
		value2, 0.0f,
		0.0f, 0.0f,
		0.0f, value2,
		value2, value2,
		value2, 0.0f
	};

	glGenBuffers(1, (GLuint*) &(planeUVs));
	glBindBuffer(GL_ARRAY_BUFFER, planeUVs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ModuleRender::DrawCubeDirect(float x, float y, float z) const
{
	float half = 0.5f;
	
	glPushMatrix();
	glTranslatef(x, y, z);

	if(actualTexture != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, *actualTexture);
	}

	glBegin(GL_TRIANGLES);

	//Back FACE
	glColor3f(30.0f, 100.0f, 200.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(half, -half, half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-half, half, half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-half, -half, half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(half, half, half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-half, half, half);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(half, -half, half);

	//Front FACE
	glColor3f(0.0f, 255.0f, 255.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, half, -half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, -half, -half);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-half, -half, -half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, half, -half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(half, half, -half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, -half, -half);
	
	//Right FACE
	glColor3f(0.0f, 0.0f, 255.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(half, half, -half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, -half, half);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(half, -half, -half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(half, half, -half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(half, half, half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, -half, half);
	
	//TOP FACE
	glColor3f(0.0f, 255.0f, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, half, half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, half, -half);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-half, half, -half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, half, half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(half, half, half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, half, -half);
	
	//RIGHT FACE
	glColor3f(255.0f, 255.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-half, -half, half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, half, -half);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-half, -half, -half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-half, half, half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, half, -half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-half, -half, half);
	
	//BOTTOM FACE
	glColor3f(255.0f, 0.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, -half, -half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, -half, half);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-half, -half, -half);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(half, -half, half);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-half, -half, half);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(half, -half, -half);
	
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void ModuleRender::DrawCubeBigArray(float x, float y, float z) const
{
	glColor3f(255.0f, 255.0f, 255.0f);

	glPushMatrix();
	glTranslatef(x, y, z);

	if(actualTexture != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, *actualTexture);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, bigArrayCube);

	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, bigArrayCubeUV);

	glTexCoordPointer(2,GL_FLOAT,0,nullptr);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void ModuleRender::DrawCubeIndices(float x, float y, float z) const
{
	glColor3f(255.0f, 255.0f, 255.0f);

	glPushMatrix();
	glTranslatef(x, y, z);

	
	if(actualTexture != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, *actualTexture);
	}
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffer);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvArrayBuffer);

	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesArrayBuffer);
	glDrawElements(GL_TRIANGLES, 36 * 3, GL_UNSIGNED_INT, nullptr); // The nullptr means 'take the last binded buffer'

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D,0);

	glPopMatrix();
}

void ModuleRender::DrawSphere(float x, float y, float z)const
{
	glMatrixMode(GL_MODELVIEW);

	glColor3f(255.0f, 255.0f, 255.0f);

	glPushMatrix();
	glTranslatef(x, y, z);

	if(actualTexture != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, *actualTexture);
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, sphereVertex);
	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, sphereUV);

	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndex);
	glDrawElements(GL_QUADS, sphereIndicesSize, GL_UNSIGNED_SHORT, nullptr);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void ModuleRender::DrawPlane(float x, float y, float z) const
{
	glColor3f(255.0f, 255.0f, 255.0f);

	glPushMatrix();
	glTranslatef(x, y, z);

	if(actualTexture != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, *actualTexture);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, planeVertices);

	glVertexPointer(3, GL_FLOAT, 0, nullptr);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, planeUVs);

	glTexCoordPointer(2, GL_FLOAT, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
}

void ModuleRender::SetUpGrid() const
{
}

void ModuleRender::DrawGrid() const
{
	glPushMatrix();

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

	glPopMatrix();
}

uint ModuleRender::LoadTexture(const char* theFileName, ILinfo* imageInfo)
{
	ILuint imageID;				// Create an image ID as a ULuint

	GLuint textureID;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(theFileName); 	// Load the image file

											// If we managed to load the image, then we can start to do things with it...
	if(success)
	{
		iluGetImageInfo(imageInfo);

		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)

		if(imageInfo != nullptr && imageInfo->Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			std::cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		if(magFilter)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);
		}

		if(minFilter)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
		}

		if(mipmap)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, mipmap);
		}

		if(anisotropicFilter)
		{
			GLfloat maxAniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

			glSamplerParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
		}

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself
	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		std::cout << "Image load failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
		exit(-1);
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	std::cout << "Texture creation successful." << std::endl;

	return (uint)textureID; // Return the GLuint to the texture so you can use it!
}

void ModuleRender::SetCurrentTexture(uint textureIndex)
{
	switch(textureIndex)
	{
		case 0: // None
			actualTexture = nullptr;
			currentImageInfo = nullptr;
			break;
		case 1: // Debug
			actualTexture = &debugTexture;
			currentImageInfo = nullptr;
			break;
		case 2: // Lenna
			actualTexture = &lenaTexture;
			currentImageInfo = lenaImageInfo;
			break;
		case 3: // Exodia
			actualTexture = &exodiaTexture;
			currentImageInfo = exodiaImageInfo;
			break;
		case 4: // Rock
			actualTexture = &rockTexture;
			currentImageInfo = rockImageInfo;
			break;
	}
}

void ModuleRender::DrawCross(const float3 & pos,float scale)const
{
	float crossSize = 0.01f * scale;
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
	glLineWidth(3);

	glBegin(GL_LINES);

	glColor3f(255, 0, 0);

	glVertex3f(-crossSize, 0, 0);
	glVertex3f(crossSize, 0, 0);

	glVertex3f(0, -crossSize, 0);
	glVertex3f(0, crossSize, 0);

	glEnd();

	glLineWidth(1);

	glPopMatrix();
}

int ModuleRender::getImageWidth() const
{
	return currentImageInfo != nullptr ? currentImageInfo->Width : -1;
}

int ModuleRender::getImageHeight() const
{
	return currentImageInfo != nullptr ? currentImageInfo->Height : -1;
}

int ModuleRender::getImageFormat() const
{
	return currentImageInfo != nullptr ? currentImageInfo->Format : -1;
}

int ModuleRender::getImageSizeBytes() const
{
	return currentImageInfo != nullptr ? currentImageInfo->SizeOfData : -1;
}

int ModuleRender::getImagePixelDepth() const
{
	return currentImageInfo != nullptr ? currentImageInfo->Depth : -1;
}

void ModuleRender::SetUpTexture()
{
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for(int i = 0; i < CHECKERS_HEIGHT; i++)
	{
		for(int j = 0; j < CHECKERS_WIDTH; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &debugTexture);
	glBindTexture(GL_TEXTURE_2D, debugTexture);

	// Set texture clamping method
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeT);

	// Set texture interpolation method to use linear interpolation (no MIPMAPS)
	if(magFilter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if(minFilter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	if(mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, mipmap);
	}

	if(anisotropicFilter)
	{
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

		glSamplerParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAniso);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void ModuleRender::SetUpTextures()
{
	CleanUpTextures();

	lenaImageInfo = new ILinfo;
	rockImageInfo = new ILinfo;
	exodiaImageInfo = new ILinfo;

	SetUpTexture();

	lenaTexture = LoadTexture("Assets/Lenna.png", lenaImageInfo);
	rockTexture = LoadTexture("Assets/rock.jpg", rockImageInfo);
	exodiaTexture = LoadTexture("Assets/exodia.dds", exodiaImageInfo);

	// Data is not valid
	// ilDeleteImages(1, &imageID);
}

void ModuleRender::CleanUpTextures()
{
	RELEASE(lenaImageInfo);
	RELEASE(rockImageInfo);
	RELEASE(exodiaImageInfo);
}

uint ModuleRender::getWrapModeS() const
{
	return wrapModeS;
}

void ModuleRender::setWrapModeS(uint wrapModeS)
{
	this->wrapModeS = wrapModeS;

	SetUpTextures();
}

uint ModuleRender::getWrapModeT() const
{
	return wrapModeT;
}

void ModuleRender::setWrapModeT(uint wrapModeT)
{
	this->wrapModeT = wrapModeT;

	SetUpTextures();
}

bool ModuleRender::getMagFilter() const
{
	return magFilter;
}

void ModuleRender::setMagFilter(bool magFilter)
{
	this->magFilter = magFilter;

	SetUpTextures();
}

bool ModuleRender::getMinFilter() const
{
	return minFilter;
}

void ModuleRender::setMinFilter(bool minFilter)
{
	this->minFilter = minFilter;

	SetUpTextures();
}

uint ModuleRender::getMagFilterMode() const
{
	return magFilterMode;
}

void ModuleRender::setMagFilterMode(uint magFilterMode)
{
	this->magFilterMode = magFilterMode;

	SetUpTextures();
}

uint ModuleRender::getMinFilterMode() const
{
	return minFilterMode;
}

void ModuleRender::setMinFilterMode(uint minFilterMode)
{
	this->minFilterMode = minFilterMode;

	SetUpTextures();
}

bool ModuleRender::getMipmap() const
{
	return mipmap;
}

void ModuleRender::setMipmap(bool mipMap)
{
	this->mipmap = mipmap;

	SetUpTextures();
}

bool ModuleRender::getAnisotropicFilter() const
{
	return anisotropicFilter;
}

void ModuleRender::setAnisotropicFilter(bool anisotropicFilter)
{
	this->anisotropicFilter = anisotropicFilter;

	SetUpTextures();
}