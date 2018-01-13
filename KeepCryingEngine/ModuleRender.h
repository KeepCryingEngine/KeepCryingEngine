#ifndef __MODULERENDER_H__
#define __MODULERENDER_H__

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

#endif // __MODULERENDER_H__