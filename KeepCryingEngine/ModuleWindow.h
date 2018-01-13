#ifndef __MODULEWINDOW_H__
#define __MODULEWINDOW_H__

#include "Module.h"

struct SDL_Window;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	virtual ~ModuleWindow();

	bool Init();

	bool CleanUp();

public:
	
	SDL_Window* window = nullptr;
};

#endif // __MODULEWINDOW_H__