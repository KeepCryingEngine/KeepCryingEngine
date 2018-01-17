#include "ModuleRender.h"

#include "GL/glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Application.h"
#include "ModuleWindow.h"

ModuleRender::ModuleRender()
{}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	// LOG("Creating Renderer context");
	bool ret = true;
	/*Uint32 flags = 0;

	if(VSYNC)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);

	if(!renderer)
	{
		// LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}*/

	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	GLenum err = glewInit();
	// … check for errors
	LOG_DEBUG("Using Glew %s", glewGetString(GLEW_VERSION));
	// Should be 2.1
	
	glcontext = SDL_GL_CreateContext(App->window->window);


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

	return ret;
}

update_status ModuleRender::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	/*SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate(float deltaTimeS, float realDeltaTimeS)
{
	//SDL_RenderPresent(renderer);
	glBegin(GL_TRIANGLES);
	glColor3f(255.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, -0.5f, 0.0f); // lower left vertex
	glVertex3f(1.0f, -0.5f, 0.0f); // lower right vertex
	glVertex3f(0.0f, 0.5f, 0.0f); // upper vertex
	glEnd();

	SDL_GL_SwapWindow(App->window->window);
	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	// LOG("Destroying renderer");

	/*if(renderer)
	{
		SDL_DestroyRenderer(renderer);
	}*/

	SDL_GL_DeleteContext(glcontext);

	return true;
}
