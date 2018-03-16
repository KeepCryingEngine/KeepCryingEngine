#include "Application.h"

#include <json.hpp>
#include <fstream>

#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleEditorUI.h"
#include "ModuleCamera.h"
#include "ModuleShader.h"
#include "ModuleScene.h"
#include "ModuleTexture.h"
#include "ModuleEntity.h"
#include "ModuleAnim.h"
#include "ModuleAudio.h"
#include "ModuleGameUI.h"
#include "ModuleFont.h"
#include "ModuleTime.h"
#include "ModuleFX.h"

using namespace std;
using nlohmann::json;

Application::Application()
{
	modules.push_back(window = new ModuleWindow());
	modules.push_back(time = new ModuleTime());
	modules.push_back(uiEditor = new ModuleEditorUI());
	modules.push_back(anim = new ModuleAnim());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(fx = new ModuleFX());
	modules.push_back(input = new ModuleInput());
	modules.push_back(uiGame = new ModuleGameUI());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(shader = new ModuleShader());
	modules.push_back(texture = new ModuleTexture());
	modules.push_back(audio = new ModuleAudio());
	modules.push_back(entity = new ModuleEntity());//After Renderer, it needs access to buffer on init
	modules.push_back(font = new ModuleFont());
}

Application::~Application()
{
	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend(); it++)
		RELEASE(*it);
}

// json stuff
// Do this in a different way
// ModuleJson / Utils

void Application::LoadConfiguration()
{
	// Update global variables

	ifstream jsonFile("Assets/Configuration.json");
	
	json json;
	
	jsonFile >> json;

	// Read: maxRealDeltaTimeS, limitFps, desiredFps


	string temp= json["title"];
	configuration.title = temp;
	configuration.vsync = json["vsync"];
	configuration.screenWidth = json["screenWidth"];
	configuration.screenHeight = json["screenHeight"];
	configuration.fullScreen = json["fullScreen"];
	configuration.maxRealDeltaTimeS = json["maxRealDeltaTimeS"];
	configuration.limitFps = json["limitFps"];
	configuration.desiredFps = json["desiredFps"];
}

bool Application::Init()
{
	LoadConfiguration();

	bool ret = true;

	desiredS = 1.0f / configuration.desiredFps;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; it++)
		ret = (*it)->Init();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; it++)
	{
		if((*it)->IsEnabled())
			ret = (*it)->Start();
	}

	return ret;
}

update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->PreUpdate();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->Update();

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->PostUpdate();

	if(configuration.limitFps)
	{
		if(time->GetEditorDeltaTime() < desiredS)
		{
			float waitingTimeS = desiredS - time->GetEditorDeltaTime();

			SDL_Delay((uint)(1000 * waitingTimeS));
		}
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->CleanUp();

	return ret;
}

void Application::Play()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		if((*it)->IsEnabled())
			(*it)->Play();
}

void Application::Pause()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		if((*it)->IsEnabled())
			(*it)->Pause();
}

void Application::Stop()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
		if((*it)->IsEnabled())
			(*it)->Stop();
}
