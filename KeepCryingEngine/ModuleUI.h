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
	update_status Update(float deltaTimeS, float realDeltaTimeS)override;
private:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

#endif // !_MODULEUI_H_

