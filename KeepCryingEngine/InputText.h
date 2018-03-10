#ifndef _INPUTTEXT_H_
#define _INPUTTEXT_H_

#include "Component.h"

class GameObject;

class InputText : public Component
{
public:
	static const Component::Type TYPE = Component::Type::InpuText;

	InputText();
	~InputText();

	void Awake() override;
	void Destroy() override;
	void SetEnable(bool value)override;

	void RealUpdate(float deltaTimeS, float realDeltaTimeS)override;

	void DrawUI() override;

	void OnFocus();

	void SetPlaceHolderGameObject(GameObject& g);
	void SetTextGameObject(GameObject& g);

	GameObject* GetPlaceHolderGameObject()const;
	GameObject* GetTextGameObject() const;

private:
	bool onFocus = false;
	GameObject * placeHolderGameObject;
	GameObject * textGameObject;
	bool dirtyText = true;
};

#endif // !_INPUTTEXT_H_

