#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>
#include "Globals.h"
#include "Module.h"

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleTime;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleTime* time;

private:

	std::list<Module*> modules;
};

extern Application* App;

#endif // !_APPLICATION_H_