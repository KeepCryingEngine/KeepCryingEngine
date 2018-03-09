#include "InputText.h"

InputText::InputText():Component(InputText::TYPE)
{}

InputText::~InputText()
{}

void InputText::SetTextGameObject(GameObject & g)
{
	textGameObject = &g;
}

GameObject * InputText::GetTextGameObject() const
{
	return textGameObject;
}
