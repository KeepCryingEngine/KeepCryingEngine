#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include <imgui.h>
#include <float3.h>

#include "Module.h"

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;
	update_status Update(float deltaTimeS, float realDeltaTimeS) override;
	update_status PostUpdate(float deltaTimeS, float realDeltaTimeS) override;

private:
	void DrawMainMenu();
	void DrawAboutMenu();
	void DrawToolsMenu();

private:
	float verticalFOV = 1.0f;
	float aspectRatio = 0.0f;
	float nearPlane = 0.1f;
	float farPlane = 50.0f;
	float movSpeed = 0.0f;
	float rotSpeed = 0.0f;
	float3 position = { 0.0f, 0.0f, 0.0f };
	float clearColor[3] = { 0.0f, 0.0f, 0.0f };
};

#endif // !_MODULEUI_H_