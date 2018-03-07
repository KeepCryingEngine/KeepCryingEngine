#ifndef _MODULEGAMEUI_H_
#define _MODULEGAMEUI_H_

#include "Module.h"

class Canvas;
class GameObject;

class ModuleGameUI : public Module
{
public:
	ModuleGameUI();
	~ModuleGameUI();

	update_status Update(float deltaTimeS, float realDeltaTimeS) override;
	bool CleanUp() override;

	void SetCanvas(Canvas& canvas);
	Canvas* GetCanvas();

private:
	void UpdateRecursive(float deltaTimeS, float realDeltaTimeS, GameObject* g);

private:
	Canvas* root = nullptr;

};

#endif // !_MODULEGAMEUI_H_

