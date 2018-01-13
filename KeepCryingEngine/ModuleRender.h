#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#include "Module.h"

struct SDL_Renderer;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

public:
	SDL_Renderer * renderer = nullptr;

};

#endif // !_MODULERENDER_H_