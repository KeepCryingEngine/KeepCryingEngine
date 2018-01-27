#ifndef _MODULEUI_H_
#define _MODULEUI_H_

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
	void CallWindows();
	void DrawMainMenu();
	void DrawAboutMenu();
	void DrawCameraWindow();
	void DrawSpeedWindow();
	void DrawStyleWindow();

private:
	//One bool control for each window
	bool cameraWindow = true;
	bool speedWindow = true;
	bool styleWindow = true;

	//Probably there's a better way of doing this thing...
	float movementSpeed = 10.0f;
	float rotationSpeed = 2.0f;
	float dragSpeed = 3.0f;
	float orbitSpeed = 3.0f;
	float zoomSpeed = 3.0f;
	float verticalFOV = 1.0f;
	float aspectRatio = 0.0f;
	float nearPlane = 0.1f;
	float farPlane = 50.0f;
	float3 position = { 0.0f, 0.0f, 0.0f };
	float3 up = { 0.0f, 0.0f, 0.0f };
	float3 front = { 0.0f, 0.0f, 0.0f };
	float3 side = { 0.0f, 0.0f, 0.0f };
	float clearColor[3] = { 0.0f, 0.0f, 0.0f };
};

#endif // !_MODULEUI_H_