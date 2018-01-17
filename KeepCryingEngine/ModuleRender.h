#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#include <SDL.h>


#include "Module.h"


struct SDL_Renderer;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init() override;
	update_status PreUpdate(float deltaTimeS, float realDeltaTimeS) override;
	update_status PostUpdate(float deltaTimeS, float realDeltaTimeS) override;
	bool CleanUp() override;

public:
	//SDL_Renderer * renderer = nullptr;
	SDL_GLContext glcontext;
};

#endif // !_MODULERENDER_H_