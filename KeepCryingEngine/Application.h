#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>

#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleUI;
class ModuleCamera;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDeltaTimeScale() const;
	void SetDeltaTimeScale(float deltaTimeScale);

private:

	void LoadConfiguration();

public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleUI* ui;
	ModuleCamera* camera;

	// Cargar información desde json
	// Tamaño pantalla, nombre, fps, etc

	struct
	{
		std::string title;
		bool vsync;
		int screenWidth;
		int screenHeight;
		bool fullScreen;
		float maxRealDeltaTimeS;
		bool limitFps;
		float desiredFps;
	} configuration;

private:

	std::list<Module*> modules;

	uint lastTimeMs = 0;

	float deltaTimeScale = 1.0f;

	float desiredS; // Computed
};

extern Application* App;

#endif // !_APPLICATION_H_