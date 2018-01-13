#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#include "Module.h"
#include "Vector2.h"
#include <SDL_scancode.h>

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum Axis 
{
	Horizontal,
	Vertical
};

class ModuleInput : public Module
{

public:

	ModuleInput();

	virtual ~ModuleInput();

	bool Init();

	bool Start();

	update_status PreUpdate();

	bool CleanUp();

	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	float GetAxis(Axis) const;

	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	bool GetWindowEvent(EventWindow code) const;

	const Vector2& GetMouseMotion() const;
	const Vector2& GetMousePosition() const;

private:
	bool		windowEvents[WE_COUNT];
	KeyState*	keyboard;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	Vector2 mouse_motion;
	Vector2 mouse;
};

#endif // !_MODULEINPUT_H_