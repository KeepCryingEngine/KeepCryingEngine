#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#define NUM_MOUSE_BUTTONS 5

#include <float2.h>

#include "Module.h"

enum class EventWindow
{
	WE_QUIT,
	WE_HIDE,
	WE_SHOW,
	WE_COUNT
};

enum class KeyState
{
	KEY_IDLE,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

enum class Axis
{
	Horizontal,
	Vertical
};

class ModuleInput : public Module
{
public:

	ModuleInput();

	virtual ~ModuleInput();

	bool Init() override;

	bool Start() override;

	update_status PreUpdate(float deltaTimeS, float realDeltaTimeS) override;

	bool CleanUp() override;

	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	bool GetKeyPressed(int id) const
	{
		if(!overUI)
		{
			KeyState keyState = GetKey(id);

			return keyState == KeyState::KEY_REPEAT || keyState == KeyState::KEY_DOWN;
		}
		else
		{
			return false;
		}
	}

	float GetAxis(Axis) const;

	KeyState GetMouseButton(int id) const
	{
		if(!overUI)
		{
			return mouse_buttons[id - 1];
		}
	}

	bool GetMouseButtonDown(int id) const
	{
		if(!overUI)
		{
			KeyState keyState = GetMouseButton(id);

			return keyState == KeyState::KEY_REPEAT || keyState == KeyState::KEY_DOWN;
		}
		else
		{
			return false;
		}
	}

	bool GetWindowEvent(EventWindow code) const;

	const float2& GetMouseMotion() const;
	const float2& GetMousePosition() const;
	const float& GetWheelMotion() const;

	void SetOverUI(bool value);
	bool GetOverUI()const;

private:
	bool windowEvents[(uint)EventWindow::WE_COUNT];
	KeyState* keyboard;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	float2 mouse_motion;
	float2 mouse;
	float wheel_motion;
	bool overUI = false;
};

#endif // !_MODULEINPUT_H_