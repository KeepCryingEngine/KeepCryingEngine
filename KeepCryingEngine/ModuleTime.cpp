#include "ModuleTime.h"

#include <SDL.h>

#include "Application.h"

ModuleTime::ModuleTime()
{}

ModuleTime::~ModuleTime()
{}

update_status ModuleTime::Update()
{
	uint currentTimeMs = SDL_GetTicks();

	realTimeDeltaTime = (currentTimeMs - lastTimeMs) / 1000.0f;
	realTimeDeltaTime = fminf(realTimeDeltaTime, App->configuration.maxRealDeltaTimeS);
	deltaTime = timeScale * deltaTime;
	lastTimeMs = currentTimeMs;

	time += deltaTime;
	realTimeSinceStartup += realTimeDeltaTime;

	frameCount = 1 / deltaTime;

	return update_status::UPDATE_CONTINUE;
}

float ModuleTime::GetFrameCount() const
{
	return frameCount;
}

float ModuleTime::GetTime() const
{
	return time;
}

float ModuleTime::GetTimeScale() const
{
	return timeScale;
}

float ModuleTime::GetDeltaTime() const
{
	return deltaTime;
}

float ModuleTime::GetRealTimeSinceStartup() const
{
	return realTimeSinceStartup;
}

float ModuleTime::GetRealTimeDeltaTime() const
{
	return realTimeDeltaTime;
}

void ModuleTime::SetTimeScale(float timeScale)
{
	this->timeScale = timeScale;
}
