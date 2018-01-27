#include "ModuleWindow.h"

#include <SDL.h>

#include "Application.h"

ModuleWindow::ModuleWindow()
{}

ModuleWindow::~ModuleWindow()
{}

bool ModuleWindow::Init()
{
	LOG_DEBUG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG_DEBUG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

		if(App->configuration.fullScreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		window = SDL_CreateWindow(App->configuration.title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, App->configuration.screenWidth, App->configuration.screenHeight, flags);

		if(!window)
		{
			LOG_DEBUG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
	}

	return ret;
}

bool ModuleWindow::CleanUp()
{
	LOG_DEBUG("Destroying SDL window and quitting all SDL systems");

	if(window)
	{
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
	return true;
}