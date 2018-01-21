#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>

#include "Globals.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleTestJson.h"
#include "ModuleTestMathGeoLib.h"
#include "ModuleUI.h"

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

	// Cargar informaci�n desde json
	// Tama�o pantalla, nombre, fps, etc

	struct
	{
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