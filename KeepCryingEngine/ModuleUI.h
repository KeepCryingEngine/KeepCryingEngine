#ifndef _MODULEUI_H_
#define _MODULEUI_H_

#include <float3.h>
#include <TextEditor.h>

#include "Module.h"
#include "GameObject.h"

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
	void SetUpTextEditor();
	void SetTextOnEditor(int shaderMode);
	void CallWindows();
	void SetAllParameters();
	void DrawMainMenu();
	void DrawAboutMenu();
	void DrawCameraWindow();
	void DrawSpeedWindow();
	void DrawStyleWindow();
	void DrawTextureInfoWindow();
	void DrawShaderWindow();
	void DrawHierarchyWindow();
	void DrawInspectorWindow();

	void PrintChildrenOnHierarchy(std::vector<GameObject*> children);

	void UpdateWrapModeS(uint wrapModeS, uint previousWrapModeS) const;
	void UpdateWrapModeT(uint wrapModeT, uint previousWrapModeT) const;

	void UpdateMagFilterMode(uint magFilterMode, uint previousMagFilterMode) const;
	void UpdateMinFilterMode(uint minFilterMode, uint previousMinFilterMode) const;

private:
	TextEditor editor;
	const char* shaderSavePath = nullptr;

	//Parameter control bools
	bool wireframeEnabled = false;
	bool antialiasingEnabled = true;
	bool textureEnabled = true;
	bool cullEnabled = true;
	bool lightningEnabled = true;
	bool depthEnabled = true;
	bool colormaterialEnabled = true;
	bool fogEnabled = false;

	//One bool control for each window
	bool cameraWindow = false;
	bool speedWindow = false;
	bool styleWindow = false;
	bool imageInfoWindow = false;
	bool shaderEditorWindow = false;
	bool hierarchyWindow = true;
	bool inspectorWindow = false;

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

	bool magFilter = false;
	bool minFilter = false;
	bool mipmap = false;
	bool anisotropicFilter = false;
	float fogDensity = 1.0f;

	std::string selectedNodeName = "";
};

#endif // !_MODULEUI_H_