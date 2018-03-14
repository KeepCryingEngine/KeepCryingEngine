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

	editorDeltaTime = (currentTimeMs - lastTimeMs) / 1000.0f;
	editorDeltaTime = fminf(editorDeltaTime, App->configuration.maxRealDeltaTimeS);

	if(state == TimeState::PLAYING)
	{
		realTimeDeltaTime = editorDeltaTime;
		deltaTime = timeScale * realTimeDeltaTime;	

		time += deltaTime;
		realTimeSinceStartup += realTimeDeltaTime;

		frameCount = 1 / deltaTime;
	}

	lastTimeMs = currentTimeMs;

	return update_status::UPDATE_CONTINUE;
}

void ModuleTime::Play()
{
	if(state == TimeState::STOPED)
	{
		time = 0;
		realTimeSinceStartup = 0;
	}

	state = TimeState::PLAYING;
}

void ModuleTime::Pause()
{
	state = TimeState::PAUSED;
}

void ModuleTime::Stop()
{
	state = TimeState::STOPED;
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

float ModuleTime::GetEditorDeltaTime() const
{
	return editorDeltaTime;
}

void ModuleTime::SetTimeScale(float timeScale)
{
	this->timeScale = fmaxf(fminf(timeScale, 1.0f), 0.0f);
}
