#include "ModuleRender.h"
#include <SDL.h>
#include "Application.h"
#include "ModuleWindow.h"

#include "GL/glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleRender::ModuleRender()
{
}

ModuleRender::~ModuleRender()
{}

bool ModuleRender::Init()
{
	// LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if(VSYNC)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);
	
	if(!renderer)
	{
		// LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

update_status ModuleRender::PreUpdate()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	SDL_RenderPresent(renderer);
	return update_status::UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
	// LOG("Destroying renderer");

	if(renderer)
	{
		SDL_DestroyRenderer(renderer);
	}

	return true;
}
