#ifndef _MODULEGAMEUI_H_
#define _MODULEGAMEUI_H_

#include "Module.h"

class Canvas;
class Image;
class GameObject;
class Component;

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

	void UpdateComponent(Component * component);

	void UpdateCanvas(Canvas* canvas);
	void UpdateImage(Image* image);

private:
	Canvas* root = nullptr;

};

#endif // !_MODULEGAMEUI_H_

