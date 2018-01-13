#ifndef _MODULETIME_H_
#define _MODULETIME_H_

#include "Module.h"
#include <SDL_timer.h>

class ModuleTime :
	public Module
{
public:
	ModuleTime(bool enabled = true);
	~ModuleTime();

	bool Init() override;
	update_status PreUpdate() override;

	float GetDeltaTime() const;
private:
	float currentDeltaTime;
	Uint32 previousTicks;
};

#endif // !_MODULETIME_H_