#ifndef _INPUTTEXT_H_
#define _INPUTTEXT_H_

#include "Component.h"
#include "GameUI.h"

class GameObject;

class InputText : public Component,public GameUI
{
public:
	static const Component::Type TYPE = Component::Type::InpuText;

	InputText();
	~InputText();

	void SetTextGameObject(GameObject& g);

	GameObject* GetTextGameObject() const;

private:
	GameObject * textGameObject;
};

#endif // !_INPUTTEXT_H_

