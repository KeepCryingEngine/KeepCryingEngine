#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include <imgui.h>

#include "Module.h"

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init() override;
	bool CleanUp() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;

private:
	void DrawMainMenu();
	void DrawAboutMenu();
	void DrawToolsMenu();

public:
	float FOV = 1.0f;
	float aspectRatio = 0.0f;
	float nearPlane = 0.0f;
	float farPlane = 0.0f;
	float movSpeed = 0.0f;
	float rotSpeed = 0.0f;

private:
	float clearColor[3] = { 0.0f, 0.0f, 0.0f };
};

#endif // !_MODULEUI_H_