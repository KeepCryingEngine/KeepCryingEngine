#include "ModuleWindow.h"

#include <SDL.h>
#include "GL/glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Globals.h"
#include "Application.h"

ModuleWindow::ModuleWindow()
{}

ModuleWindow::~ModuleWindow()
{}

bool ModuleWindow::Init()
{
	// LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		// LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{

		int width = SCREEN_WIDTH;
		int height = SCREEN_HEIGHT;
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

		if(FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(!window)
		{
			// LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
	}

	return ret;
}

bool ModuleWindow::CleanUp()
{
	// LOG("Destroying SDL window and quitting all SDL systems");

	if(window)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
	return true;
}

