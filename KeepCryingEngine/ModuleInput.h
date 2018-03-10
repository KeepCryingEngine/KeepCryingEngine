#ifndef _MODULEINPUT_H_
#define _MODULEINPUT_H_

#define NUM_MOUSE_BUTTONS 5

#include <float2.h>

#include "Module.h"

#define MAX_TEXT 255

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
		if(!overUI)
		{
			return keyboard[id];
		}
		else
		{
			return KeyState::KEY_IDLE;
		}
	}

	KeyState UIGetKey(int id) const
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

	bool UIGetKeyPressed(int id) const
	{
		KeyState keyState = GetKey(id);

		return keyState == KeyState::KEY_REPEAT || keyState == KeyState::KEY_DOWN;
	}

	float GetAxis(Axis) const;
	float UIGetAxis(Axis) const;

	KeyState GetMouseButton(int id) const
	{
		if(!overUI)
		{
			return mouse_buttons[id - 1];
		}
		else
		{
			return KeyState::KEY_IDLE;
		}
	}

	KeyState UIGetMouseButton(int id) const
	{
		return mouse_buttons[id - 1];
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

	bool UIGetMouseButtonDown(int id) const
	{
		KeyState keyState = GetMouseButton(id);

		return keyState == KeyState::KEY_REPEAT || keyState == KeyState::KEY_DOWN;
	}

	bool GetWindowEvent(EventWindow code) const;

	const float2& GetMouseMotion() const;
	const float2& GetMousePosition() const;
	const float& GetWheelMotion() const;
	const float& UIGetWheelMotion() const;

	void SetOverUI(bool value);
	bool GetOverUI()const;

	void SetStartToRead(bool value);
	bool GetStartToRead()const;

	void SetText(const char*  newText);
	const std::string& GetCurrentText();

private:
	bool windowEvents[(uint)EventWindow::WE_COUNT];
	KeyState* keyboard;
	KeyState mouse_buttons[NUM_MOUSE_BUTTONS];
	float2 mouse_motion;
	float2 mouse;
	float wheel_motion;
	bool overUI = false;
	bool startToRead = false;
	std::string text;
};

#endif // !_MODULEINPUT_H_