#ifndef _MODULERENDER_H_
#define _MODULERENDER_H_

#define HAVE_M_PI
#define _CRTDBG_MAP_ALLOC

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
	void setUpSphere(float radius, unsigned int rings, unsigned int sectors);
	void drawCubeDirect(float x, float y, float z) const;
	void drawCubeBigArray(float x, float y, float z) const;
	void drawCubeIndices(float x, float y, float z) const;
	void drawSphere(float x, float y, float z) const;


public:
	SDL_GLContext glcontext;

private:
	uint bigArrayCube = 0;
	uint vertexArrayBuffer = 0;
	uint indicesArrayBuffer = 0;
	uint sphereIndex = 0;
	uint sphereVertex = 0;

	int sphereIndicesSize = 0;
};

#endif // !_MODULERENDER_H_