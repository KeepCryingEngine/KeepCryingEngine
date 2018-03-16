#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <list>

#include "Globals.h"

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleEditorUI;
class ModuleGameUI;
class ModuleCamera;
class ModuleShader;
class ModuleScene;
class ModuleTexture;
class ModuleEntity;
class ModuleAnim;
class ModuleAudio;
class ModuleFont;
class ModuleTime;
class ModuleFX;

class Application
{
public:

	Application();
	virtual ~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void Play();
	void Pause();
	void Stop();

private:
	void LoadConfiguration();

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleEditorUI* uiEditor = nullptr;
	ModuleGameUI* uiGame = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleShader* shader = nullptr;
	ModuleScene* scene = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleAnim* anim = nullptr;
	ModuleAudio* audio = nullptr;
	ModuleEntity* entity = nullptr;
	ModuleFont* font = nullptr;
	ModuleTime* time = nullptr;
	ModuleFX* fx = nullptr;

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

	float desiredS;
};

extern Application* App;

#endif // !_APPLICATION_H_