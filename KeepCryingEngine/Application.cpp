#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleTime.h"
#include "ModuleTestJson.h"
#include "ModuleTestMathGeoLib.h"
#include "ModuleUI.h"

using namespace std;

Application::Application()
{
	modules.push_back(input = new ModuleInput());
	modules.push_back(window = new ModuleWindow());
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(time = new ModuleTime());
	modules.push_back(ui = new ModuleUI());

	modules.push_back(new ModuleTestJson());
	modules.push_back(new ModuleTestMathGeoLib());
}

Application::~Application()
{
	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend(); it++)
		RELEASE(*it);
}

// json stuff
// Do this in a different way
// ModuleJson / Utils

#include <json.hpp>
#include <fstream>

using namespace std;
using nlohmann::json;

void Application::loadConfiguration()
{
	// Update global variables

	ifstream jsonFile("Assets/Configuration.json");
	
	json json;
	
	jsonFile >> json;

	// Read: maxRealDeltaTimeS, limitFps, desiredFps

	configuration.maxRealDeltaTimeS = json["maxRealDeltaTimeS"];
	configuration.limitFps = json["limitFps"];
	configuration.desiredFps = json["desiredFps"];
}

bool Application::Init()
{
	loadConfiguration();

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

	uint currentTimeMs = SDL_GetTicks();

	float realDeltaTimeS = (currentTimeMs - lastTimeMs) / 1000.0f;

	// limit realDeltaTimeS
	realDeltaTimeS = fminf(realDeltaTimeS, configuration.maxRealDeltaTimeS);

	float deltaTimeS = deltaTimeScale * realDeltaTimeS;

	//LOG_DEBUG("%f, %i, %f", realDeltaTimeS, configuration.limitFps, desiredS); // Test

	lastTimeMs = currentTimeMs;

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->PreUpdate(deltaTimeS, realDeltaTimeS);

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->Update(deltaTimeS, realDeltaTimeS);

	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == update_status::UPDATE_CONTINUE; ++it)
		if((*it)->IsEnabled())
			ret = (*it)->PostUpdate(deltaTimeS, realDeltaTimeS);

	if(configuration.limitFps)
	{
		if(realDeltaTimeS < desiredS)
		{
			float waitingTimeS = desiredS - realDeltaTimeS;

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

float Application::getDeltaTimeScale() const
{
	return deltaTimeScale;
}

void Application::setDeltaTimeScale(float deltaTimeScale)
{
	this->deltaTimeScale = fmaxf(fminf(deltaTimeScale, 1.0f), 0.0f);
}