#ifndef _MODULE_H_
#define _MODULE_H_

#include "Globals.h"

class Module
{
public:

	Module(bool enabled = true) : enabled(enabled)
	{}

	virtual ~Module()
	{}

	bool IsEnabled() const
	{
		return enabled;
	}

	bool Enable()
	{
		if(!enabled)
			return enabled = Start();

		return true;
	}

	bool Disable()
	{
		if(enabled)
			return enabled = !CleanUp();

		return true;
	}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual void Play(){}
	virtual void Pause(){}
	virtual void Unpause(){}
	virtual void Stop(){}

private:

	bool enabled = true;
};

#endif // !_MODULE_H_