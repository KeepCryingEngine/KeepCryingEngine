#ifndef _MODULEWINDOW_H_
#define _MODULEWINDOW_H_

#include "Module.h"

struct SDL_Window;

class ModuleWindow : public Module
{
public:

	ModuleWindow();

	virtual ~ModuleWindow();

	bool Init() override;

	bool CleanUp() override;

public:

	SDL_Window* window = nullptr;
};

#endif // !_MODULEWINDOW_H_