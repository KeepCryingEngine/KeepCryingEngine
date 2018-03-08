#ifndef _MODULEGAMEUI_H_
#define _MODULEGAMEUI_H_

#include "Module.h"

class Canvas;
class Image;
class Button;
class Text;
class InputText;
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
	GameObject* GetFocusGameObject();
	GameObject* GetHoveringGameObject();

private:
	void UpdateRecursive(float deltaTimeS, float realDeltaTimeS, GameObject* g);

	void UpdateComponent(Component * component);

	void CheckUIStatus();

	void PreOrdenZCheck(GameObject* currentNode);

	bool CheckIfMouseOver(GameObject* g);

	void UpdateCanvas(Canvas* canvas);
	void UpdateImage(Image* image);
	void UpdateButton(Button* button);
	void UpdateText(Text* text);
	void UpdateInputText(InputText* inputText);

private:
	Canvas* root = nullptr;
	GameObject* focus = nullptr;
	GameObject* hovering = nullptr;

};

#endif // !_MODULEGAMEUI_H_

