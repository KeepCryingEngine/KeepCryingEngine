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

private:
	void setupBigArray() const;
	void setupIndicesArray() const;
	void drawCubeDirect() const;
	void drawCubeBigArray() const;
	void drawCubeIndices() const;

public:
	SDL_GLContext glcontext;

private:
	uint bigArrayCube = 0;
	uint vertexArrayBuffer = 0;
	uint indicesArrayBuffer = 0;
};

#endif // !_MODULERENDER_H_