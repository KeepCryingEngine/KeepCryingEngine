#include "ModuleInput.h"

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <ImGuizmo.h>

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "Camera.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput() : Module(), mouse({ 0, 0 }), mouse_motion({ 0,0 }),wheel_motion(0)
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (uint)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	memset(mouse_buttons, (uint)KeyState::KEY_IDLE, sizeof(KeyState) * NUM_MOUSE_BUTTONS);
}

ModuleInput::~ModuleInput()
{
	RELEASE_ARRAY(keyboard);
}

bool ModuleInput::Init()
{
	LOG_DEBUG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG_DEBUG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleInput::Start()
{
	return true;
}

update_status ModuleInput::PreUpdate(float deltaTimeS, float realDeltaTimeS)
{
	static SDL_Event event;

	SetOverUI(false);

	mouse_motion = { 0, 0 };
	wheel_motion = 0;
	memset(windowEvents, false, (uint)EventWindow::WE_COUNT * sizeof(bool));

	const Uint8* keys = SDL_GetKeyboardState(nullptr);

	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KeyState::KEY_IDLE)
				keyboard[i] = KeyState::KEY_DOWN;
			else
				keyboard[i] = KeyState::KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
				keyboard[i] = KeyState::KEY_UP;
			else
				keyboard[i] = KeyState::KEY_IDLE;
		}
	}

	for(int i = 0; i < NUM_MOUSE_BUTTONS; ++i)
	{
		if(mouse_buttons[i] == KeyState::KEY_DOWN)
			mouse_buttons[i] = KeyState::KEY_REPEAT;

		if(mouse_buttons[i] == KeyState::KEY_UP)
			mouse_buttons[i] = KeyState::KEY_IDLE;
	}

	// If mouse is hovering any window ignore Button down events
	// Button up and Motion events are not ignored

	bool mouseHoveringAnyWindow = ImGui::IsMouseHoveringAnyWindow();
	bool mouseHoveringAnyGuizmo = ImGuizmo::IsOver();

	if(startToRead)
	{
		SDL_StartTextInput();
	}
	while(SDL_PollEvent(&event) != 0)
	{
		ImGui_ImplSdlGL3_ProcessEvent(&event);

		switch(event.type)
		{
			case SDL_QUIT:
				windowEvents[(uint)EventWindow::WE_QUIT] = true;
				break;

			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
					//case SDL_WINDOWEVENT_LEAVE:
					case SDL_WINDOWEVENT_RESIZED:
						SDL_GetWindowSize(App->window->window, &App->configuration.screenWidth, &App->configuration.screenHeight);
						App->camera->camera->SetAspectRatio((float)App->configuration.screenWidth/(float)App->configuration.screenHeight);
						break;

					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_FOCUS_LOST:
						windowEvents[(uint)EventWindow::WE_HIDE] = true;
						break;

						//case SDL_WINDOWEVENT_ENTER:
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_FOCUS_GAINED:
					case SDL_WINDOWEVENT_MAXIMIZED:
					case SDL_WINDOWEVENT_RESTORED:
						windowEvents[(uint)EventWindow::WE_SHOW] = true;
						break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				if(!mouseHoveringAnyWindow && !mouseHoveringAnyGuizmo)
				{
					mouse_buttons[event.button.button - 1] = KeyState::KEY_DOWN;
				}
				break;

			case SDL_MOUSEBUTTONUP:
				mouse_buttons[event.button.button - 1] = KeyState::KEY_UP;
				break;

			case SDL_MOUSEMOTION:
				mouse_motion.x = (float)event.motion.xrel;
				mouse_motion.y = (float)event.motion.yrel;
				mouse.x = (float)event.motion.x;
				mouse.y = (float)event.motion.y;
				break;
			case SDL_MOUSEWHEEL:
				if(!mouseHoveringAnyWindow)
				{
					wheel_motion = (float)event.wheel.y;
				}
				break;

			case SDL_KEYDOWN:
			{
				if(startToRead)
				{
					//Handle backspace
					if(event.key.keysym.sym == SDLK_BACKSPACE && text.length() > 0)
					{
						//lop off character
						if(actualTextPos > 0)
						{
							text.erase(actualTextPos - 1, 1);
							if(--actualTextPos < 0)
							{
								actualTextPos++;
							}
						}
						
					}
					//Handle copy
					else if(event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
					{
						SDL_SetClipboardText(text.c_str());
					}
					//Handle paste
					else if(event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
					{
						text = SDL_GetClipboardText();
					}
					else if(event.key.keysym.sym == SDLK_RIGHT)
					{
						if(++actualTextPos > text.length())
						{
							actualTextPos--;
						}
					}
					else if(event.key.keysym.sym == SDLK_LEFT)
					{
						if(--actualTextPos < 0)
						{
							actualTextPos++;
						}
					}
				}
			}
			break;
			case SDL_TEXTINPUT:
			{
				if(startToRead)
				{
					text.insert(actualTextPos++, event.text.text);
				}
			}
			break;
		}
	}

	SetStartToRead(false);

	if(GetWindowEvent(EventWindow::WE_QUIT) || GetKey(SDL_SCANCODE_ESCAPE) == KeyState::KEY_DOWN)
		return update_status::UPDATE_STOP;

	return update_status::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG_DEBUG("Quitting SDL event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

float ModuleInput::GetAxis(Axis axis) const
{
	float ret = 0.0f;
	if(!overUI)
	{
		if(axis == Axis::Horizontal)
		{
			ret -= GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT ? 1 : 0;
			ret += GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT ? 1 : 0;
		}
		else
		{
			ret -= GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT ? 1 : 0;
			ret += GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT ? 1 : 0;
		}
	}
	return ret;
}

float ModuleInput::UIGetAxis(Axis axis) const
{
	float ret = 0.0f;
	if(axis == Axis::Horizontal)
	{
		ret -= GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT ? 1 : 0;
		ret += GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT ? 1 : 0;
	}
	else
	{
		ret -= GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT ? 1 : 0;
		ret += GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT ? 1 : 0;
	}
	return ret;
}

bool ModuleInput::GetWindowEvent(EventWindow ev) const
{
	return windowEvents[(uint)ev];
}

const float2& ModuleInput::GetMousePosition() const
{
	return mouse;
}

const float & ModuleInput::GetWheelMotion() const
{
	if(!overUI)
	{
		return wheel_motion;
	}
	else
	{
		return 0;
	}
}

const float & ModuleInput::UIGetWheelMotion() const
{
	return wheel_motion;
}

void ModuleInput::SetOverUI(bool value)
{
	overUI = value;
}

bool ModuleInput::GetOverUI() const
{
	return overUI;
}

void ModuleInput::SetStartToRead(bool value)
{
	startToRead = value;
}

bool ModuleInput::GetStartToRead() const
{
	return startToRead;
}

void ModuleInput::SetText(const char* newText)
{
	text = newText;
	actualTextPos = text.length();
}

const std::string& ModuleInput::GetCurrentText()
{
	return text;
}

const float2& ModuleInput::GetMouseMotion() const
{
	return mouse_motion;
}