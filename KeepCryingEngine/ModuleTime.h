#ifndef _MODULETIME_H_
#define _MODULETIME_H_

#include "Module.h"

enum class TimeState
{
	STOPED,
	PAUSED,
	PLAYING
};

class ModuleTime : public Module
{
public:
	ModuleTime();
	~ModuleTime();

	update_status Update() override;

	void Play();
	void Pause();
	void Stop();

	float GetFrameCount() const;
	float GetTime() const;
	float GetTimeScale() const;
	float GetDeltaTime() const;
	float GetRealTimeSinceStartup() const;
	float GetRealTimeDeltaTime() const;

	float GetEditorDeltaTime()const;

	void SetTimeScale(float timeScale);

private:
	float frameCount = 0;
	float time = 0;
	float timeScale = 1;
	float deltaTime = 0;
	float realTimeSinceStartup = 0;
	float realTimeDeltaTime = 0;

	float editorDeltaTime = 0;

	TimeState state = TimeState::STOPED;

	uint lastTimeMs = 0;
};

#endif // !_MODULETIME_H_

