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

private:
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

#endif // !_MODULEUI_H_